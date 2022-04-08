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

void type_prompt();
void read_command(char *command, char **arguments, int *num_args);
void execute_command(char *command, char **arguments, int *num_args);
void redirect_io(char *command, char **arguments, int *num_args);
void cd(char *path);

void shell_loop() {
    char cmnd[100], command[1000], *arguments[20];
    while(1) {
        int num_args = 0;

        type_prompt();
        read_command(command, arguments, &num_args);
        
        pid_t pid = fork();

        if (pid == 0) {
            execute_command(command, arguments, &num_args);
            exit(0);
        }
        else {
            // waitpid(getpid(), NULL, WNOHANG);
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

void read_command(char *command, char **arguments, int *num_args) {
    char input[100];
    scanf(" %100[^\n]", input);

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

int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    shell_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
