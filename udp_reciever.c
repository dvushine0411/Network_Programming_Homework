#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main() {
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (receiver == -1) {
        perror("Lỗi tạo socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    if (bind(receiver, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Bind thất bại");
        return 1;
    }

    char buff[256];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    printf("UDP Echo Server đang đợi dữ liệu tại cổng 8000...\n");

    while(1) {
        int n = recvfrom(receiver, buff, sizeof(buff) - 1, 0, 
                         (struct sockaddr*)&client_addr, &client_addr_len);
        
        if (n > 0) {
            buff[n] = '\0';
            printf("Nội dung nhận được: %s\n", buff);
            sendto(receiver, buff, n, 0, 
                   (struct sockaddr *)&client_addr, client_addr_len);
        }
    }

    close(receiver);
    return 0;
}