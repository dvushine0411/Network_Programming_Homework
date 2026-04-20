#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
typedef struct {
    char mssv[50];
    char hoTen[100];
    char ngaySinh[50];
    double diemTB;

} SinhVien;

int main(int argc, char* argv[]) {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    connect(client, (struct sockaddr *)&addr, sizeof(addr));

    SinhVien sv;
    printf("Nhập MSSV: ");
    scanf("%s", sv.mssv);
    getchar();
    printf("Nhập họ tên: ");
    fgets(sv.hoTen, sizeof(sv.hoTen), stdin);
    sv.hoTen[strcspn(sv.hoTen, "\n")] = 0;
    printf("Nhập ngày sinh: ");
    scanf("%s", sv.ngaySinh);
    printf("Nhập điểm: ");
    scanf("%lf", &sv.diemTB);

    send(client, &sv, sizeof(sv), 0);

    close(client);


}