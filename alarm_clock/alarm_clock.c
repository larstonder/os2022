#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

struct alarm
{
    time_t alarm_time;
    struct tm orig_time;
    pid_t pid;
    int id;
};

const int max_alarms = 10;
struct alarm alarms[10];
int num_alarms = 0;

void add_item(struct alarm *p, struct alarm a, int *num_items)
{
    if (*num_items < 10)
    {
        p[*num_items] = a;
        *num_items += 1;
    }
}

void delete_item(struct alarm *p, int *num_items, int item, int is_kill)
{
    if (is_kill == 1){
        kill(p[item].pid, SIGKILL);
    }
    if (*num_items > 0 && item < *num_items && item > -1)
    {
        int last_index = *num_items - 1;
        for (int i = item; i < last_index; i++)
        {
            p[i] = p[i + 1];
        }
        *num_items -= 1;

        for (int i = 0; i < *num_items; i++){
            p[i].id = i;
        }
    }
}

void print_alarm(struct tm a)
{
    printf("%d-%d-%d %d:%d:%d", a.tm_year + 1900, a.tm_mon + 1, a.tm_mday, a.tm_hour, a.tm_min, a.tm_sec);
}

void print_alarms()
{
    if (num_alarms == 0){
        printf("\nNo alarms scheduled.\n");
        return;
    }
    for (int i = 0; i < num_alarms; i++)
    {
        printf("\nAlarm %i is scheduled at: ", i+1);
        print_alarm(alarms[i].orig_time);
    }
    printf("\n");
}

void fork_alarm(struct alarm a)
{
    time_t current_time;
    time(&current_time);

    int seconds_to_alarm = (int) difftime(a.alarm_time, current_time);

    sleep(seconds_to_alarm);

    printf("Ring ring.\n");
    //Only works in Linux, not WSL unless PulseAudio is installed
    execlp("mpg123", "mpg123", "-q", "./alarm.mp3", NULL);
}

void schedule()
{
    if (num_alarms == max_alarms){
        printf("\nMaximum number of alarms reached.\n");
        return;
    }

    char time_string[20];
    printf("Schedule alarm at which date and time?:");
    scanf(" %19[^\n]", time_string);

    struct tm temp_time;
    strptime(time_string, "%Y-%m-%d %H:%M:%S", &temp_time);
    temp_time.tm_hour++;
    time_t new_alarm_time = mktime(&temp_time);

    time_t current_time;
    time(&current_time);

    if (new_alarm_time < current_time)
    {
        printf("New alarm must be after current time.\n");
        return;
    }

    pid_t pid = fork();

    struct alarm new_alarm;
    new_alarm.alarm_time = new_alarm_time;
    new_alarm.orig_time = temp_time;
    new_alarm.pid = pid;
    new_alarm.id = num_alarms;

    if (pid == 0)
    {
        fork_alarm(new_alarm);
        exit(0);
    }
    else {
        waitpid(getpid(), NULL, WNOHANG);
    }

    add_item(alarms, new_alarm, &num_alarms);

    printf("Scheduling alarm in %i seconds.\n", (int) difftime(new_alarm_time, current_time));
}

void cancel(){
    if (num_alarms == 0){
        printf("\nNo alarms active.\n");
        return;
    }

    int cancel_alarm;
    printf("\nDelete which alarm?: ");
    scanf("%d", &cancel_alarm);

    if (cancel_alarm > num_alarms){
        printf("\nThis alarm does not exist.\n");
        return;
    }
    delete_item(alarms, &num_alarms, cancel_alarm-1, 1);
}

void update_alarms(){
    time_t current_time;
    time(&current_time);
    int k;
    for (int i = num_alarms-1; i >= 0; i--) {
        if (waitpid(alarms[i].pid, &k, WNOHANG)){
            if (WIFEXITED(k)){
                delete_item(alarms, &num_alarms, i, 0);
            }
        }
    }
}

int main()
{
    time_t t;
    struct tm *info;
    time(&t);
    info = gmtime(&t);

    printf("Welcome to the alarm clock! It is currently %d-%d-%d %d:%d:%d.\n",
           info->tm_year + 1900, info->tm_mon + 1, info->tm_mday, (info->tm_hour + 1) % 24, info->tm_min, info->tm_sec);

    char input;

    while (input != 'x')
    {
        printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit):\n");
        scanf("%s", &input);
        switch (input)
        {
        case 's':
            schedule();
            break;
        case 'l':
            update_alarms();
            print_alarms();
            break;
        case 'c':
            cancel();
            break;
        default:
            break;
        }
    }
    printf("Goodbye.\n");

    return 0;
}