#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_CLIENTS 64
#define PORT 9000

struct Client {
    int fd;
    int state; // 0: Chua dang nhap, 1: Da dang nhap
};

// Hàm kiểm tra tài khoản từ file database.txt
int check_login(const char* user_pass) {
    FILE *f = fopen("database.txt", "r");
    if (!f) {
        perror("Khong the mo database.txt");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // Xóa ký tự xuống dòng ở cuối mỗi dòng đọc được
        line[strcspn(line, "\r\n")] = 0; 
        
        if (strcmp(line, user_pass) == 0) {
            fclose(f);
            return 1; // Khớp tài khoản
        }
    }
    
    fclose(f);
    return 0; // Không tìm thấy
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    listen(listener, 5);
    printf("Telnet Server dang chay tren cong %d...\n", PORT);

    struct Client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = 0;
        clients[i].state = 0;
    }

    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(listener, &readfds);
        int max_fd = listener;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > max_fd) max_fd = clients[i].fd;
            }
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        // 1. CÓ CLIENT MỚI KẾT NỐI
        if (FD_ISSET(listener, &readfds)) {
            int new_fd = accept(listener, NULL, NULL);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd == 0) {
                    clients[i].fd = new_fd;
                    clients[i].state = 0; // Trạng thái 0: Yêu cầu đăng nhập
                    char *msg = "Vui long nhap tai khoan (VD: admin admin):\n> ";
                    send(new_fd, msg, strlen(msg), 0);
                    break;
                }
            }
        }

        // 2. CÓ DỮ LIỆU TỪ CLIENT CŨ
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i].fd;
            
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int valread = read(sd, buf, sizeof(buf) - 1);

                if (valread <= 0) {
                    // Client ngắt kết nối
                    close(sd);
                    clients[i].fd = 0;
                } else {
                    // Dọn dẹp ký tự thừa (Enter) do client gửi lên
                    buf[strcspn(buf, "\r\n")] = '\0';
                    
                    if (strlen(buf) == 0) continue; // Bỏ qua nếu gửi chuỗi rỗng

                    // XỬ LÝ TRẠNG THÁI 0: ĐĂNG NHẬP
                    if (clients[i].state == 0) {
                        if (check_login(buf)) {
                            clients[i].state = 1; // Chuyển sang trạng thái đã đăng nhập
                            char *msg = "=> Dang nhap thanh cong! Ban co the nhap lenh:\n> ";
                            send(sd, msg, strlen(msg), 0);
                        } else {
                            char *msg = "=> Sai user hoac pass. Vui long thu lai:\n> ";
                            send(sd, msg, strlen(msg), 0);
                        }
                    } 
                    // XỬ LÝ TRẠNG THÁI 1: THỰC THI LỆNH HỆ THỐNG
                    else if (clients[i].state == 1) {
                        char cmd[1200];
                        char filename[32];
                        
                        // Đặt tên file out riêng biệt cho từng client (tránh ghi đè chéo nhau)
                        sprintf(filename, "out_%d.txt", sd);
                        
                        // Lắp ráp lệnh: "lệnh_của_client > out_fd.txt 2>&1"
                        // 2>&1 để gom cả lỗi (stderr) vào chung file text
                        sprintf(cmd, "%s > %s 2>&1", buf, filename);
                        
                        // Gọi hệ điều hành thực thi lệnh
                        system(cmd);

                        // Đọc kết quả từ file out và gửi trả lại client
                        FILE *f = fopen(filename, "r");
                        if (f) {
                            char file_buf[2048];
                            int bytes_read;
                            while ((bytes_read = fread(file_buf, 1, sizeof(file_buf), f)) > 0) {
                                send(sd, file_buf, bytes_read, 0);
                            }
                            fclose(f);
                            remove(filename); // Chạy xong thì xóa file rác đi
                        }
                        
                        send(sd, "\n> ", 3, 0); // In lại dấu nhắc lệnh
                    }
                }
            }
        }
    }
    return 0;
}