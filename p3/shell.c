///////////////////////////////////
/* The FAMOUS LITTLE UNIX SHELL™ */
/* Made by: Vegard, Lars & Axel  */
///////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

void type_prompt(); //TODO: Define function
void read_command(char *command, char **arguments); //TODO: Define function

void shell_loop() {
    char cmnd[100], command[1000], *arguments[20];
    while(1) {
        type_prompt();
        read_command(command, arguments);
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

void read_command(char *command, char **arguments) {
    char input[100];
    int i = 0;
    scanf(" %100[^\n]", input);

    char *token = strtok(input, " \t");
    *strcpy(command, token);

    while (token != NULL){
        token = strtok(NULL, " \t");
        
        arguments[i++] = token;
    }

    for (int j = 0; j < i-1; j++){
        printf("%s\n", arguments[j]);
    }
}

int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    shell_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
