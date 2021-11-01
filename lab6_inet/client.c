#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define LEN_MESSAGE 128
#define SOCKET_ADDRESS "localhost"
#define SOCKET_PORT 7000
#define QNT_MESSAGE 3

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket error\n");
        exit(-1);
    }

    struct hostent* host = gethostbyname(SOCKET_ADDRESS);

    if (!host)
    {
        perror("Getting host by name error\n");
        close(sock);
        exit(-1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCKET_PORT);
    server_addr.sin_addr = *((struct in_addr*) host->h_addr_list[0]);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connect error\n");
        close(sock);
        return -1;
    }
    char message[LEN_MESSAGE];

    for(int i = 0; i < QNT_MESSAGE; i++) {
        memset(message, 0, LEN_MESSAGE);
        sprintf(message, "Message %d for stream socket from client with pid %d ", i + 1, getpid());

        if (send(sock, message, strlen(message), 0) < 0)
        {
            perror("Send error\n");
            close(sock);
            return -1;
        }
        sleep(rand() %5 + 1);
    }
    close(sock);
    return 0;
}