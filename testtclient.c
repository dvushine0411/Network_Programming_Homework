#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Tạo socket
    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1) {
        perror("socket() failed");
        return 1;
    }

    // 2. Khai báo địa chỉ server (localhost, port 8080)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    // 3. Kết nối tới Server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() failed");
        close(client_sock);
        return 1;
    }

    char buffer[1024];
    char input[256];
    int bytes_received;

    // --- BƯỚC 1: NHẬN YÊU CẦU NHẬP TÊN VÀ GỬI ĐI ---
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        printf("%s", buffer); // In ra "Vui long nhap Ho va Ten: "
        
        // Đọc từ bàn phím
        fgets(input, sizeof(input), stdin);
        
        // Gửi lên server
        send(client_sock, input, strlen(input), 0);
    }

    // --- BƯỚC 2: NHẬN YÊU CẦU NHẬP MSSV VÀ GỬI ĐI ---
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        printf("%s", buffer); // In ra "Vui long nhap MSSV: "
        
        // Đọc từ bàn phím
        fgets(input, sizeof(input), stdin);
        
        // Gửi lên server
        send(client_sock, input, strlen(input), 0);
    }

    // --- BƯỚC 3: NHẬN KẾT QUẢ EMAIL ---
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        printf("------------------------------------\n");
        printf("Server tra ve: %s", buffer);
        printf("------------------------------------\n");
    }

    // 4. Đóng kết nối
    printf("Hoan tat. Dong ket noi.\n");
    close(client_sock);

    return 0;
}