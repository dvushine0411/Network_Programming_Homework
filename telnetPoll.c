#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <poll.h>
#include <unistd.h>
#include <stdbool.h>

const int PORT = 8080;
const int MAX_CLIENTS = 1000;

struct ClientInfo {
    int state;

};

bool checkAuth(const char* user_pass) {
    FILE *f = fopen("database.txt", "r");
    if (!f)
    {
        perror("cannot open database.txt");
        return false;
    }

    char line[256];
    while(fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, user_pass) == 0)
        {
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;

}


int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(listener == -1)
    {
        perror("listener() failed");
        return 1;
    }

    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)))
    {
        perror("setsockopt() failed");
        close(listener);
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        close(listener);
        return 1;
    }

    listen(listener, 5);

    printf("Server is running at port: %d\n", PORT);

    struct ClientInfo Clients[MAX_CLIENTS];
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 0;
    fds[0].fd = listener;
    fds[0].events = POLLIN;

    nfds++;
    

    while(1)
    {
        int ret = poll(fds, nfds, -1);

        if(ret < 0)
        {
            perror("poll() failed");
            break;
        }

        // Khi có kết nối đến server //
        if(fds[0].revents && POLLIN)
        {
            int newFD = accept(listener, NULL, NULL);
            if(newFD >= 0)
            {
                if(nfds < MAX_CLIENTS)
                {
                    fds[nfds].fd = newFD;
                    fds[nfds].events = POLLIN;
                    Clients[nfds].state = 0;

                    char *msg = "Vui lòng nhập tài khoản và mật khẩu";
                    send(newFD, msg, strlen(msg), 0);

                    printf("Client FD %d đã kết nối\n", newFD);
                    nfds++;
                }
            }
            else
            {
                close(newFD);
            }

        }

        for(int i = 1; i < nfds; i++)
        {
            if(fds[i].revents && POLLIN)
            {
                char buf[1024];
                int valread = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);

                if(valread <= 0)
                {
                    printf("Client FD %d ngắt kết nối\n", fds[i].fd);
                    close(fds[i].fd);

                    fds[i] = fds[nfds - 1];
                    Clients[i] = Clients[nfds - 1];
                    nfds--;
                    i--;
                }
                else
                {
                    buf[valread] = '\0';
                    buf[strcspn(buf, "\n")] = 0;
                    if(strlen(buf) == 0)   continue;

                    if(Clients[i].state == 0)
                    {
                        if(checkAuth(buf))
                        {
                            Clients[i].state = 1;
                            char *msg = "Đăng nhập thành công\n";
                            send(fds[i].fd, msg, strlen(msg), 0);

                        }
                        else
                        {
                            char *msg = "Đăng nhập thất bại! Vui lòng thử lại\n";
                            send(fds[i].fd, msg, strlen(msg), 0);
                        }

                    }
                    else if(Clients[i].state == 1)
                    {
                        char cmd[1200];
                        char filename[32];

                        sprintf(filename, "out_%d.txt", fds[i].fd);

                        sprintf(cmd, "%s > %s 2>&1", buf, filename);

                        system(cmd);

                        FILE *f = fopen(filename, "r");
                        if(f)
                        {
                            char file_buf[2048];
                            int bytesRead;
                            while((bytesRead = fread(file_buf, 1, sizeof(file_buf), f)) > 0)
                            {
                                send(fds[i].fd, file_buf, bytesRead, 0);
                            
                            }
                            fclose(f);
                            remove(filename);

                        }
                        send(fds[i].fd, "\n> ", 3, 0);
                    }
                }



            }
        }


    }


}