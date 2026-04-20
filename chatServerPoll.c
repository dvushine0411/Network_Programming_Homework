#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <time.h>

const int PORT = 8090;
const int MAX_CLIENTS = 10000;


struct ClientInfo {
    int state;
    char id[64];
    char name[64];

};


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

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if(bind(listener, (struct sockaddr*)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        close(listener);
        return 1;
    }

    listen(listener, 5);

    printf("Server is listenning on PORT %d\n", PORT);

    struct ClientInfo clients[MAX_CLIENTS];
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;
    fds[0].fd = listener;
    fds[0].events = POLLIN;

    while(1)
    {
        int ret = poll(fds, nfds, -1);
        if(ret < 0)
        {
            perror("Poll failed");
            break;
        }
        
        // Sự kiện có khách kết nối đến server //
        if(fds[0].revents && POLLIN)
        {
            int newFd = accept(listener, NULL, NULL);
            if(newFd >= 0)
            {
                if(nfds < MAX_CLIENTS)
                {
                    fds[nfds].fd = newFd;
                    fds[nfds].events = POLLIN;

                    clients[nfds].state = 0;
                    memset(clients[nfds].id, 0, sizeof(clients[nfds].id));
                    memset(clients[nfds].name, 0, sizeof(clients[nfds].name));

                    char *prompt = "Vui lòng nhập tên theo cú pháp 'client_id: client_name': \n";
                    send(newFd, prompt, strlen(prompt), 0);

                    printf("Client mới kết nối! FD: %d\n", newFd);
                    nfds++;

                }
                else
                {
                    close(newFd);
                }
            }
        }

        // Sự kiện có tin nhắn từ khách đã được kết nối //
        for(int i = 1; i < nfds; i++)
        {
            if(fds[i].revents && POLLIN)
            {
                char buf[1024];
                int len = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);

                // Sự kiện có ngắt kết nối //
                if(len <= 0)
                {
                    printf("Client FD %d ngắt kết nối.\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    clients[i] = clients[nfds - 1];

                    nfds--;
                    i--;
                }

                // Sự kiện client gửi tin nhắn //
                else 
                {
                    buf[len] = '\0';
                    buf[strcspn(buf, "\n")] = 0;

                    if(strlen(buf) == 0)   continue;

                    // Xử lý theo trạng thái của client //
                    // Trạng thái 1: Chờ đăng nhập //
                    if(clients[i].state == 0)
                    {
                        char tmp_id[64];
                        char tmp_name[64];
                        if(sscanf(buf, "%s %s", tmp_id, tmp_name) == 2)
                        {
                            clients[i].state = 1;
                            strcpy(clients[i].id, tmp_id);
                            strcpy(clients[i].name, tmp_name);

                            char *ok = "Dang nhap thanh cong! Ban co the bat dau chat:\n";
                            send(fds[i].fd, ok, strlen(ok), 0);
                        }
                        else 
                        {
                            char *err = "Sai cu phap. Vui long nhap lai (VD: abc: Hung):\n";
                            send(fds[i].fd, err, strlen(err), 0);
                        }
                    } 

                    // Nếu mà người dùng đã đăng nhập vào //

                    else if(clients[i].state == 1)
                    {
                        time_t t = time(NULL);
                        struct tm *tm_info = localtime(&t);
                        char time_str[32];
                        strftime(time_str, sizeof(time_str), "%Y/%m/%d %I:%M:%S%p", tm_info);

                        char out_buf[2048];
                        snprintf(out_buf, sizeof(out_buf), "%s %s: %s\n", time_str, clients[i].id, buf);

                        for(int j = 1; j < nfds; j++)
                        {
                            if(j != i && clients[j].state == 1)
                            {
                                send(fds[j].fd, out_buf, strlen(out_buf), 0);
                            }
                        }

                    }
                }
            }
        }


    }
    close(listener);
    return 0;


}