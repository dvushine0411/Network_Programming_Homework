#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(client == -1)
    {
        printf("Lỗi không tạo được socket\n");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int testConnect = connect(client, (struct sockaddr*)&addr, sizeof(addr));

    if(testConnect == -1)
    {
        printf("Gặp lỗi khi kết nối với server");
    }

    while(1)
    {
        char buff[256];
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = 0;
        send(client, buff, strlen(buff), 0);
    }

    close(client);
    
}