#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {

    int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == -1) {
        perror("socket() failed");
        return 1;
    }

   
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() failed");
        close(client_sock);
        return 1;
    }

    char buffer[1024];
    char input[256];
    int bytes_received;

    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        printf("%s", buffer); 
        
        fgets(input, sizeof(input), stdin);
        
        send(client_sock, input, strlen(input), 0);
    }


    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        printf("%s", buffer);
        
        fgets(input, sizeof(input), stdin);
        
      
        send(client_sock, input, strlen(input), 0);
    }

    
    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        printf("------------------------------------\n");
        printf("Server tra ve: %s", buffer);
        printf("------------------------------------\n");
    }

    
    printf("Hoan tat. Dong ket noi.\n");
    close(client_sock);

    return 0;
}