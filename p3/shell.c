///////////////////////////////////
/* The FAMOUS LITTLE UNIX SHELL™ */
/* Made by: Vegard, Lars & Axel  */
///////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

struct Node {
    int data;
    char* command;
    struct Node* next;
};

void type_prompt();
void read_command(char *command, char **arguments, int *num_args, int *is_background);
void execute_command(char *command, char **arguments, int *num_args);
void redirect_io(char *command, char **arguments, int *num_args);
void add_to_list(struct Node *element);
void remove_from_list(int pid);
void kill_zombies();
void cd(char *path);

struct Node tail;
struct Node head;

void shell_loop() {
    char cmnd[100], command[1000], *arguments[20];

    while(1) {
        int num_args = 0;
        int is_background;

        kill_zombies();
        type_prompt();
        read_command(command, arguments, &num_args, &is_background);
        
        pid_t pid = fork();

        if (pid == 0) {
            execute_command(command, arguments, &num_args);
            exit(0);
        }
        else if (is_background == 1) {
            struct Node new_node;
            new_node.data = pid;
            new_node.command = command;
            add_to_list(&new_node);
            waitpid(getpid(), NULL, WNOHANG);
        }
        else {
            wait(NULL);
        }

        if(strcmp(command, "exit") == 0) break;
    }
}

void type_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s: ", cwd);
    } else {
        perror("getcwd() error");
    }
}

void read_command(char *command, char **arguments, int *num_args, int *is_background) {
    char input[100];
    scanf(" %100[^\n]", input);

    // int cmpbg = strcmp(input[strlen(input)-1], '&');
    if (input[strlen(input)-1] == '&'){
        *is_background = 1;
        input[strlen(input)-1] = '\0';
    }
    else {
        *is_background = 0;
    }

    char *token = strtok(input, " \t");
    *strcpy(command, token);

    while (token != NULL){
        token = strtok(NULL, " \t");
        
        arguments[(*num_args)++] = token;
    }

    (*num_args)--;

    int cmp = strcmp("cd", command);
    if (cmp == 0) {cd(arguments[0]);}
}

void execute_command(char *command, char **arguments, int *num_args){
    int cmpcd = strcmp("cd", command);

    if (cmpcd != 0) {
        redirect_io(command, arguments, num_args);
    }

    char **arguments_copy;
    int num_args_copy = 0;

    for(int i = 0; i < *num_args; i++){
        int cmpin = strcmp(arguments[i], "<");
        int cmpout = strcmp(arguments[i], ">");
        if (cmpin == 0 || cmpout == 0) {
            *num_args -= 2;
            continue;
        }
        strcpy(arguments_copy[num_args_copy], arguments[i]);
        num_args_copy++;
    }

    execvp(command, arguments);

    printf("Exit status [%s",command);

    for (int i = 0; i < *num_args; i++){
        printf(" %s", arguments[i]);
    }
    printf("] = 0\n");
}

void cd(char *path){
    printf("%s\n", path);
    if (chdir(path) != 0) perror("Could not change path");
}

void redirect_io(char *command, char **arguments, int *num_args){
    for (int i = *num_args-1; i >= 0; i--){
        int cmpin = strcmp(arguments[i], "<");
        int cmpout = strcmp(arguments[i], ">");
        if (cmpin == 0){
            int fd0 = open(arguments[i+1], O_RDONLY, 0);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
        }
        else if (cmpout == 0){
            int fd1 = creat(arguments[i+1], 0644);
            dup2(fd1, STDIN_FILENO);
            close(fd1);
        }
    }
}

void add_to_list(struct Node *element){
    element->next = head.next;
    head.next = element;
    return;
}

void remove_from_list(int pid){
    struct Node current = head;
    while(current.next != NULL){
        printf("Hello\n");
        struct Node next = *current.next;
        if (next.data == pid){
            current.next = next.next;
        }
        else {
            current = next;
        }
        
    }
}

void kill_zombies(){
    int k;
    int i = waitpid(-1, &k, WNOHANG);
    if (WIFEXITED(k)){
        remove_from_list(i);
        kill_zombies();
    }
}

int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    tail.command = "TAIL";
    tail.data = -2;
    tail.next = NULL;

    head.command = "HEAD";
    head.data = -2;
    head.next = &tail;
    shell_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
