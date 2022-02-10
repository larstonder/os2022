#define _XOPEN_SOURCE
#include <stdio.h>
#include <time.h>

struct alarm {
    time_t alarm_time;
    int id;
};

struct alarm alarms[10];
int id = 0;

void schedule(){
    char time_string[20];
    printf("Schedule alarm at which date and time?:");
    scanf("%19s", time_string);
    struct tm temp_time;
    strptime(time_string, "%Y-%m-%d %H:%M:%S", &temp_time);
    time_t new_alarm_time = mktime(&temp_time);
    struct alarm new_alarm;
    new_alarm.alarm_time = new_alarm_time;
    new_alarm.id = id;
    id++;
}

int main(){
    time_t t;
    struct tm *info;
    time(&t);
    info = gmtime(&t);

    printf("Welcome to the alarm clock! It is currently %d-%d-%d %d:%d:%d.\n",
        info->tm_year+1900, info->tm_mon+1, info->tm_mday, (info->tm_hour+1)%24, info->tm_min, info->tm_sec);

    char input;

    while (input != 'x'){
        printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit):\n");
        scanf("%s", &input);
        switch (input)
        {
        case 's':
            schedule();
            break;
        case 'l':
            printf("Listing current alarms.\n");
            break;
        case 'c':
            printf("Alarm canceled.\n");
            break;
        default:
            break;
        }
    }
    printf("Goodbye.\n");

    return 0;
}