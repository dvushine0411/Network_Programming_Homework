#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int main() {
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sender == -1)
    {
        printf("Lỗi không tạo được socket\n");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8000);

    char buff[256];
    char recv_buff[256];
    while(1)
    {
        printf("Nhập dữ liệu cần điền vào:");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = 0;
        sendto(sender, buff, strlen(buff), 0, (struct sockaddr*)&addr, sizeof(addr));

        int n = recvfrom(sender, recv_buff, sizeof(recv_buff) - 1, 0, NULL, NULL);

        if(n > 0)
        {
            recv_buff[n] = '\0';
            printf("Server gửi lại: %s\n", recv_buff);
        }

    }

    close(sender);

}