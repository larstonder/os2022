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

typedef struct Node {
    int data;
    char* command;
    struct Node* next;
} Node;

void type_prompt();
void read_command(char *command, char **arguments, int *num_args, int *is_background);
void execute_command(char *command, char **arguments, int *num_args, int exit_status);
void redirect_io(char *command, char **arguments, int *num_args);
void add_to_list(char* command, int pid);
void remove_from_list(int pid);
void kill_zombies();
void print_jobs();
int zombie_stat(int pid);
void cd(char *path);

struct Node tail;
struct Node head;

void shell_loop() {

    while(1) {
        int num_args = 0;
        int is_background = 0;
        char command[1000], *arguments[20];

        kill_zombies();
        type_prompt();
        read_command(command, arguments, &num_args, &is_background);

        pid_t pid = fork();

        int exit_status = 0;

        if (pid == 0) {
            execute_command(command, arguments, &num_args, exit_status);
            exit(0);
        }
        else if (is_background == 1) {
            add_to_list(command, pid);
            waitpid(getpid(), &exit_status, WNOHANG);
        }
        else {
            wait(&exit_status);
        }

        if(strcmp(command, "exit") == 0) {
            printf("\n");
            break;
        }
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

    int cmpjobs = strcmp("jobs", command);
    if (cmpjobs == 0) {print_jobs();}
}

void execute_command(char *command, char **arguments, int *num_args, int exit_status) {
    int cmpcd = strcmp("cd", command);
    int cmpjobs = strcmp("jobs", command);

    if (cmpcd != 0 || cmpjobs != 0) {
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
    printf("] = %d\n", exit_status);
}

void cd(char *path) {
    // printf("%s\n", path);
    if (chdir(path) != 0) perror("Could not change path");
}

void redirect_io(char *command, char **arguments, int *num_args) {
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

void add_to_list(char* command, int pid) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = pid;
    new_node->command = malloc(sizeof(command));
    strcpy(new_node->command, command);
    new_node->next = head.next;
    head.next = new_node;
    return;
}

void kill_zombies() {
    struct Node *current = &head;
    while(current->next != NULL){
        struct Node *next = current->next;
        if (next->data != -2){
            int k = zombie_stat(next->data);
            if (k != -1){
                current->next = next->next;
                free(next);
            }
        }
        current = next;
    }
}

int zombie_stat(int pid) {
    int k;
    if (waitpid(pid, &k, WNOHANG))
    {
        if (WIFEXITED(k))
        {
            return WEXITSTATUS(k);
        }
    }
    return -1;
}

void print_jobs() {
    struct Node current = head;
    while(current.next != NULL){
        printf("Command: %s, PID: %d\n", current.command, current.data);
        current = *current.next;
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
