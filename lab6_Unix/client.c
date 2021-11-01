#include "info.h"

int main()
{
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("Socket error\n");
        return(sock);
    }

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_NAME);

    char message[LEN_MESSAGE];
    sprintf(message, "Message from  client with pid %d", getpid());
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr *)&server, sizeof(server));

    close(sock);
    return 0;
}