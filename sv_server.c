#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h> 

typedef struct {
    char mssv[50];
    char hoTen[100];
    char ngaySinh[50];
    double diemTB;
} SinhVien;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Sử dụng: %s <cổng> <file_log>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *log_filename = argv[2];

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Bind failed");
        return 1;
    }

    listen(listener, 5);
    printf("Server đang chạy trên cổng %d, ghi log vào file %s...\n", port, log_filename);

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);
        
        int client = accept(listener, (struct sockaddr *)&client_addr, &client_addrlen);

        SinhVien sv;
        memset(&sv, 0, sizeof(sv));

        int received = recv(client, &sv, sizeof(sv), 0);

        if(received <= 0) {
            close(client);
            continue;
        }

       
        char *client_ip = inet_ntoa(client_addr.sin_addr);

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char time_str[26];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        FILE *f = fopen(log_filename, "a");
        if (f != NULL) {
            
            printf("%s %s %s %s %s %.2f\n", 
                    client_ip, time_str, sv.mssv, sv.hoTen, sv.ngaySinh, sv.diemTB);

           
            fprintf(f, "%s %s %s %s %s %.2f\n", 
                    client_ip, time_str, sv.mssv, sv.hoTen, sv.ngaySinh, sv.diemTB);
            
            fclose(f);
        }

        close(client);
    }
    
    close(listener);
    return 0;
}