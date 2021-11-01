#include "info.h"

int sock;



int main(void)
{
    char message[LEN_MESSAGE];
    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        perror("Socket error\n");
        return(sock);
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_NAME);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind error\n");
        close(sock);
        unlink(SOCKET_NAME);
        return(-1);
    }

    printf("\nServer is waiting for message\n");

    for (;;)
    {
        int size = recv(sock, message, sizeof(message), 0);

        if (size < 0)
        {
            perror("Recv error\n");
            close(sock);
            unlink(SOCKET_NAME);
            return(size);
        }
        message[size] = 0;
        printf("New message: %s\n", message);
    }

}