#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>




int main() {
    // Tạo socket //
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    // Khai báo địa chỉ server //
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    // bind //
    int res = bind(listener, (struct sockaddr *)&addr, sizeof(addr));

    // listen - Chuyển socket sang trạng thái chờ kết nối //
    int a = listen(listener, 5);

    if(a == -1)
    {
        printf("Không thể tạo kết nối đến máy client\n");
    }

    // accept chấp nhận kết nối //
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);
    int client_sk = accept(listener, (struct sockaddr *)&client_addr, &client_addrlen);

    if(client_sk == -1)
    {
        printf("Lỗi khi chấp nhận kết nối\n");
    }

    // Hàm recv: Nhận dữ liệu từ client gửi về //
    char buf[256];
    while(1)
    {
        int ret = recv(client_sk, buf, sizeof(buf), 0);
        if(ret <= 0)
        {
            printf("CLient đã ngắt kết nối\n");
        }
        buf[ret] = '\0';
        printf("Dữ liệu nhận được: %s", buf);
    }
    close(client_sk);
    close(listener);


}