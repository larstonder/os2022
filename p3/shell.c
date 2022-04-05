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

void type_prompt();
void read_command(char *command, char **arguments, int *num_args);
void execute_command(char *command, char **arguments, int *num_args);

void shell_loop() {
    char cmnd[100], command[1000], *arguments[20];
    int num_args = 0;
    while(1) {
        type_prompt();
        read_command(command, arguments, &num_args);
        
        pid_t pid = fork();

        if (pid == 0) {
            execute_command(command, arguments, &num_args);
            exit(0);
        }
        else {
            waitpid(getpid(), NULL, WNOHANG);
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
}

void execute_command(char *command, char **arguments, int *num_args){
    printf("Exit status [%s",command);
    for (int i = 0; i < *num_args; i++){
        printf(" %s", arguments[i]);
    }
    printf("] = 0\n");
}

int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    shell_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
