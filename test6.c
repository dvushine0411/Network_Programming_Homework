#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h> 

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener < 0) {
        perror("Không thể tạo socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    // Kiểm tra lỗi Bind
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Lỗi Bind (Có thể cổng 9000 đang bị dùng)");
        close(listener);
        return 1;
    }

    if (listen(listener, 5) < 0) {
        perror("Lỗi Listen");
        return 1;
    }

    printf("Waiting for client...\n");
    
    int client = accept(listener, NULL, NULL);
    if (client < 0) {
        perror("Lỗi Accept");
    } else {
        printf("Client connected!\n");
        close(client); // Đóng client sau khi xong
    }

    close(listener); // Dùng close thay vì pclose
    return 0;
}