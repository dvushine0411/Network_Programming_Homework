
// Bài 1:
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>


int main(int argc, char *argv[]) {
    // Khai báo socket //
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Khai báo địa chỉ server //
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(argv[2]);

    // Kết nối đến server //
    int res = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if(res == -1)
    {
        printf("Không thể kết nối đến server\n");
    }
    char msg[256];
    while(1)
    {
        fgets(msg, sizeof(msg), stdin);
        send(client, msg, strlen(msg), 0);
    }
    
}