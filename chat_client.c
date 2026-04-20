#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nLoi tao socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nDia chi IP khong hop le \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nKet noi that bai. Hay chac chan Server dang chay!\n");
        return -1;
    }

    printf("=> Da ket noi thanh cong den Server!\n");

    fd_set readfds;
    int max_fd = sock;

    while (1) {
        FD_ZERO(&readfds);
        
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Loi select");
            break;
        }

        if (FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, 1024);
            int valread = read(sock, buffer, 1024);
            
            if (valread <= 0) {
                printf("\n[He thong] Server da dong ket noi hoac bi loi.\n");
                break;
            }
            
            printf("\r\033[K"); 
            printf("%s", buffer);
            
            printf("> ");
            fflush(stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            memset(buffer, 0, 1024);
            int bytes_read = read(STDIN_FILENO, buffer, 1024);
            
            if (bytes_read > 0) {
                send(sock, buffer, bytes_read, 0);
                printf("> ");
                fflush(stdout);
            }
        }
    }

    close(sock);
    return 0;
}