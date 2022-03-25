///////////////////////////////////
/* The FAMOUS LITTLE UNIX SHELL™ */
/* Made by: Vegard, Lars & Axel  */
///////////////////////////////////

#include <stdlib.h>
#include <string.h>

void type_prompt(); //TODO: Define function
void read_command(char *command, char **arguments); //TODO: Define function

void shell_loop() {
    char cmnd[100], command[1000], *arguments[20];
    while(1) {
        type_prompt();
        read_command(command, arguments);
        if(strcmp(command, "exit") ==0) break;
    }
}

int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    shell_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}
