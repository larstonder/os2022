#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "bbuffer/bbuffer.h"

#define SIZE 1024
#define MAXREQ (4096 * 1024)

body[MAXREQ], msg[MAXREQ];

typedef struct
{
    char method[10];
    char path[30];
} FirstLine;

char body[4096 * 1024];

int parseRequest(char request[], FirstLine *fl)
{
    char method[10];
    char path[30];

    char *token = strtok(request, " ");
    strcpy(method, token);
    token = strtok(NULL, " ");
    strcpy(path, token);
    path[strcspn(path, "\n")] = 0;

    printf("%s", path);

    if (strcmp(method, "GET") == 0)
    {
        printf("greater success\n");
        strcpy(fl->method, method);
        strcpy(fl->path, path);
        return 1;
    }
    return 0;
}

void init_threadpool(int n_threads, pthread_t *threadpool, void *thread_function){
    for (int i = 0; i < n_threads; i++){
        pthread_create(threadpool[i], NULL, thread_function, NULL);
    }
}

void process_request(){
    
}

int main(int argc, char *argv[])
{
    char *www_path = argv[1];
    int port = (int) strtol(argv[2], (char **)NULL, 10);
    int n_threads = (int) strtol(argv[3], (char **)NULL, 10);
    int bufferslots = (int) strtol(argv[4], (char **)NULL, 10);

    BNDBUF *buffer = bb_init(bufferslots);

    pthread_t threadpool[n_threads];

    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char client_message[2000];
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    while(1){
        // Clean buffers:
        memset(msg, '\0', sizeof(msg));
        memset(client_message, '\0', sizeof(client_message));

        // Listen for clients:
        if (listen(socket_desc, 1) < 0)
        {
            printf("Error while listening\n");
            return -1;
        }
        printf("\nListening for incoming connections.....\n");
        
        // Accept an incoming connection:
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

        if (client_sock < 0)
        {
            printf("Can't accept\n");
            return -1;
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive client's message:
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0)
        {
            printf("Couldn't receive\n");
            return -1;
        }
        // printf("Msg from client: %s\n", client_message);
        bb_add(buffer, client_message);

        FirstLine firstLine;

        if (parseRequest(client_message, &firstLine) == 1)
        {
            char source[SIZE + 1];
            char finalPath[] = ".";
            printf("%s", finalPath);

            strcat(finalPath, www_path);
            strcat(finalPath, firstLine.path);

            printf("%s", finalPath);
            FILE *fp = fopen(finalPath, "r");
            if (fp != NULL) {
                size_t newLen = fread(source, sizeof(char), SIZE, fp);
                if ( ferror( fp ) != 0 ) {
                    fputs("Error reading file", stderr);
                } else {
                    source[newLen++] = '\0'; /* Just to be safe. */
                }

                fclose(fp);
            }

            snprintf(msg, sizeof(msg),
                "HTTP/1.0 200 OK\n"
                "Content-Type: text/html\n"
                "Content-Length: %ld\n\n%s",
                strlen(source), source);

            int n = write(client_sock, msg, strlen(msg));
        }
        else
        {
            strcpy(msg, "Error 400");
        }

        if (send(client_sock, msg, strlen(msg), 0) < 0)
        {
            printf("Can't send\n");
            return -1;
        }

        // Closing the socket:
        close(client_sock);
    }

    close(socket_desc);

    return 0;
}