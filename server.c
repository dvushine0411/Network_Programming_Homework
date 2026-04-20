#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int Check_Xau(char *buff, char *target) {
    int count = 0;
    char *ptr = buff;
    int target_len = strlen(target);
    while ((ptr = strstr(ptr, target)) != NULL) {
        count++;
        ptr += target_len; 
    }
    return count;
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("Socket error");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    listen(listener, 5);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);
        int client = accept(listener, (struct sockaddr *)&client_addr, &client_addrlen);

        char buff[256];
        char target[] = "0123456789";
        int t_len = strlen(target);
        
        char prevTail[11] = ""; 
        int cnt = 0;
        int received;

        while ((received = recv(client, buff, sizeof(buff) - 1, 0)) > 0) {
            buff[received] = '\0';

            char overlap[30];
            char head_new[11];
            strncpy(head_new, buff, t_len - 1);
            head_new[t_len - 1] = '\0';
            
            sprintf(overlap, "%s%s", prevTail, head_new);
            
            if (strstr(overlap, target) != NULL) {
                cnt++;
            }

            cnt += Check_Xau(buff, target);

            if (received >= t_len - 1) {
                strcpy(prevTail, buff + received - (t_len - 1));
            } else {
                char tempTail[512];
                sprintf(tempTail, "%s%s", prevTail, buff);
                int total_tail_len = strlen(tempTail);
                if (total_tail_len >= t_len - 1) {
                    strcpy(prevTail, tempTail + total_tail_len - (t_len - 1));
                } else {
                    strcpy(prevTail, tempTail);
                }
            }

            printf("Số lượng: %d\n", cnt);
        }

        printf("Client ngat ket noi. Tong tim thay: %d\n", cnt);
        close(client);
    }

    close(listener);
}