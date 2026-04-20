#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#define PORT 8080 // Chú ý: Port của bài Telnet là 9000

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[2048] = {0};

    // 1. Tạo Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n=> Loi tao socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. Chuyển đổi IP
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n=> Dia chi IP khong hop le \n");
        return -1;
    }

    // 3. Kết nối đến Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n=> Ket noi that bai. Hay chac chan Telnet Server dang chay!\n");
        return -1;
    }

    // 4. Chuẩn bị mảng pollfd
    struct pollfd fds[2];
    
    // Vị trí 0: Bàn phím (STDIN)
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    
    // Vị trí 1: Mạng (Socket)
    fds[1].fd = sock;
    fds[1].events = POLLIN;

    // 5. Vòng lặp chính
    while (1) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) {
            perror("Loi poll");
            break;
        }

        // SỰ KIỆN 1: SERVER GỬI DỮ LIỆU TỚI (Ví dụ: Kết quả lệnh ls, báo cáo đăng nhập)
        if (fds[1].revents & POLLIN) {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(sock, buffer, sizeof(buffer) - 1);
            
            if (valread <= 0) {
                printf("\n[He thong] Server da dong ket noi. Chuong trinh ket thuc.\n");
                break;
            }
            
            // In thẳng những gì Server gửi ra màn hình
            printf("%s", buffer);
            
            // Ép hệ điều hành in ra ngay lập tức (Rất quan trọng để hiển thị dấu "> " liền mạch)
            fflush(stdout); 
        }

        // SỰ KIỆN 2: BẠN GÕ LỆNH TỪ BÀN PHÍM
        if (fds[0].revents & POLLIN) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
            
            if (bytes_read > 0) {
                // Gửi nguyên xi chuỗi vừa gõ (bao gồm cả phím Enter) lên Server
                send(sock, buffer, bytes_read, 0);
            }
        }
    }

    close(sock);
    return 0;
}