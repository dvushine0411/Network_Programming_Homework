#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define MAX_CLIENTS 64
#define PORT 8080

struct Client {
    int fd;
    int state;
    char name[64];
};

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    listen(listener, 5);

    struct Client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = 0;
    }

    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(listener, &readfds);
        int max_fd = listener;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > max_fd) {
                    max_fd = clients[i].fd;
                }
            }
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(listener, &readfds)) {
            int new_fd = accept(listener, NULL, NULL);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd == 0) {
                    clients[i].fd = new_fd;
                    clients[i].state = 0;
                    char *msg = "Vui long nhap ten theo cu phap 'client_id: client_name':\n";
                    send(new_fd, msg, strlen(msg), 0);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i].fd;
            
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int valread = read(sd, buf, sizeof(buf) - 1);

                if (valread <= 0) {
                    close(sd);
                    clients[i].fd = 0;
                } else {
                    buf[strcspn(buf, "\r\n")] = '\0';

                    if (clients[i].state == 0) {
                        char temp_id[64];
                        char temp_name[64];
                        
                        if (sscanf(buf, "%63[^:]: %63s", temp_id, temp_name) == 2) {
                            
                            strcpy(clients[i].name, temp_id); 
                            clients[i].state = 1;

                            char *success = "=> Dang nhap thanh cong! Ban co the bat dau chat:\n";
                            send(sd, success, strlen(success), 0);
                        } else {
                            char *err = "Sai cu phap. Vui long nhap lai (VD: abc: Hung):\n";
                            send(sd, err, strlen(err), 0);
                        }
                    } 
                    else if (clients[i].state == 1) {
                        time_t t = time(NULL);
                        struct tm *tm_info = localtime(&t);
                        char time_str[32];
                        strftime(time_str, sizeof(time_str), "%Y/%m/%d %I:%M:%S%p", tm_info);

                        char send_buf[2048];
                        snprintf(send_buf, sizeof(send_buf), "%s %s: %s\n", time_str, clients[i].name, buf);

                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (clients[j].fd > 0 && clients[j].state == 1 && j != i) {
                                send(clients[j].fd, send_buf, strlen(send_buf), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}