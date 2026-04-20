#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>



int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(client == -1)
    {
        perror("client() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8090);

    if(connect(client, (struct sockaddr*)&addr, sizeof(addr)))
    {
        perror("connect() failed");
        close(client);
        return 1;
    }

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = client;
    fds[1].events = POLLIN;

    while(1)
    {
        int ret = poll(fds, 2, -1);
        if(ret < 0)
        {
            perror("poll error");
            break;
        }
        char buffer[1024];


        // Sự kiện gõ phím //
        if(fds[0].revents && POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytes_read = read(STDIN_FILENO, buffer, 1024);
            if(bytes_read > 0)
            {
                send(client, buffer, bytes_read, 0);
                printf("> ");
                fflush(stdout);

            }

        }

        if(fds[1].revents && POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(client, buffer, 1024);
            if(valread < 0)
            {
                printf("Server đã đóng kết nối\n");
                break;
            }

            printf("\r\033[K");
            printf("%s\n", buffer);
            
            printf("> ");
            fflush(stdout);
        }
    }

}