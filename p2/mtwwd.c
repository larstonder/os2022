#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct
{
    char method[10];
    char path[30];
} FirstLine;

int parseRequest(char request[], FirstLine *fl)
{
    char method[10];
    char path[30];

    char *token = strtok(request, " ");
    strcpy(method, token);
    token = strtok(NULL, " ");
    strcpy(path, token);
    path[strcspn(path, "\n")] = 0;

    if (strcmp(method, "GET") == 0)
    {
        printf("greater success\n");
        strcpy(fl->method, method);

        char dots[] = ".";
        strcat(dots, path);
        printf("%s\n", dots);

        strcpy(fl->path, dots);
        return 1;
    }
    return 0;
}

void send_file(FILE *fp, int sockfd)
{
    int n;
    char data[2000] = {0};

    while (fgets(data, 2000, fp) != NULL)
    {
        if (send(sockfd, data, sizeof(data), 0) == -1)
        {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, 2000);
    }
}

int main(void)
{
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];

    // Clean buffers:
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

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
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

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
    printf("Msg from client: %s\n", client_message);

    FirstLine firstLine;

    if (parseRequest(client_message, &firstLine) == 1)
    {
        FILE *f;
        f = fopen(firstLine.path, "r");
        if (f == NULL)
        {
            perror("[-]Error in reading file.");
            exit(1);
        }
        fscanf(f,"%s",server_message);
        write(socket_desc, server_message, 100);
    }
    else
    {
        strcpy(server_message, "Error 400");
    }

    if (send(client_sock, server_message, strlen(server_message), 0) < 0)
    {
        printf("Can't send\n");
        return -1;
    }

    // Closing the socket:
    close(client_sock);
    close(socket_desc);

    return 0;
}