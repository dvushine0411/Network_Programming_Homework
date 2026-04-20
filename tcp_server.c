// Bài 2:

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


typedef struct {
    char mssv[50];
    char hoTen[100];
    char ngaySinh[50];
    double diemTB;

} SinhVien;


int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    bind(listener, (struct sockaddr *)&addr, sizeof(addr));

    listen(listener, 5);

    while(1)
    {
        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
        
        int client = accept(listener, (struct sockaddr *)&client_addr, &client_addrlen);

        SinhVien sv;
        memset(&sv, 0, sizeof(sv));

        int recieved = recv(client, &sv, sizeof(sv), 0);

        if(recieved <= 0)
        {
            printf("Lỗi nhận dữ liệu\n");
        }
        else
        {
            printf("MSSV: %s\n", sv.mssv);
            printf("Họ tên: %s\n", sv.hoTen);
            printf("Ngày sinh: %s\n", sv.ngaySinh);
            printf("Điểm TB: %.2f\n", sv.diemTB);
            printf("----------------------------------\n");
        }

    }





}