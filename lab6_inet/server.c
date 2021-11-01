#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define LEN_MESSAGE 128
#define MAX_CLIENTS 10
#define SOCKET_ADDRESS "localhost"
#define SOCKET_PORT 7000


int clients [MAX_CLIENTS] = { 0 };
int listener_sock ;

int main( void )
{
    listener_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (listener_sock < 0)
    {
        perror("Error in sock()\n");
        return -1;
    }

    struct sockaddr_in addr ;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCKET_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Error in bind ()\n") ;
        close(listener_sock);
        return -1;
    }

    if (listen(listener_sock , 3) < 0)
    {
        perror("Error in listen ()\n");
        close(listener_sock);
        return -1;
    }

    printf("Server started on ip: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    for (;;)
    {
        fd_set fds;
        int max_fd;
        FD_ZERO(&fds);
        FD_SET(listener_sock, &fds);
        max_fd = listener_sock;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] > 0)
            {
                FD_SET(clients[i], &fds);
            }

            if (clients[i] > max_fd)
            {
                max_fd = clients[i];
            }
        }

        if (select(max_fd + 1, &fds , NULL, NULL, NULL) < 0)
        {
            perror("Error in select ()\n");
            for ( int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i] != 0)
                {
                    close(clients[i]);
                }
            }
            close(listener_sock);
            return -1;
        }

        if (FD_ISSET(listener_sock, &fds))
        {

            struct sockaddr_in client_addr;
            int client_size = sizeof(client_addr);
            int client_sock = accept(listener_sock, (struct sockaddr*)&client_addr , (socklen_t*) &client_size);

            if (client_sock < 0)
            {
                perror ("Error in accept ()\n");
                for ( int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (clients[i] != 0)
                    {
                        close(clients[i]);
                    }
                }
                close(listener_sock);
                return -1;
            }

            printf("\nServer got new connection:\nip = %s:%d\n", inet_ntoa( client_addr.sin_addr), ntohs(client_addr.sin_port));

            for ( int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i] == 0)
                {
                    clients[i] = client_sock;
                    printf("Client number − %d\n", i + 1);
                    break;
                }
            }
        }

        for ( int i = 0; i < MAX_CLIENTS; i++)
        {
            if ((clients[i] > 0) && FD_ISSET(clients[i], &fds))
            {
                char msg[LEN_MESSAGE];
                struct sockaddr_in client_addr;
                int addr_size = sizeof(client_addr);
                int msg_size = recv(clients[i] , msg, LEN_MESSAGE, 0);

                if (msg_size == 0)
                {
                    getpeername(clients[i], (struct sockaddr*)&client_addr, (socklen_t*)&addr_size);
                    printf("\nClient %d closed connection %s:%d \n", i + 1,
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(clients[i]);
                    clients[i] = 0;
                }
                else
                {
                    msg[msg_size] = '\0';
                    printf ("\nNew message from client %d: %s\n" , i + 1, msg);
                }
            }
        }
    }
}