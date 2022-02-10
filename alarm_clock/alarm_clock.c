#include <stdio.h>
#include <time.h>

int main(){

    time_t t;
    time(&t);

    printf("Welcome to the alarm clock! It is currently %s.", ctime(&t));
    
    return 0;
}