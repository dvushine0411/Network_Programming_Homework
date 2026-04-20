#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>


// Test nhắn với duy nhất 1 ứng dụng ở bên khác //

struct PeerInfo {
    struct sockaddr_in clientAddr;
    char message[256];

};


int main(int argc, char *argv[]) {
    int port_s = atoi(argv[1]);
    char *ip_d = argv[2];
    int port_d = atoi(argv[3]);

    int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in myAddr;
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(port_s);

    if(bind(udp_socket, (struct sockaddr*)&myAddr, sizeof(myAddr)))
    {
        perror("Failed binding");
        return 1;
    }

    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_addr.s_addr = inet_addr(ip_d);
    destAddr.sin_port = htons(port_d);

    unsigned long ul = 1;
    // ép socket mạng thành non-blocking //
    ioctl(udp_socket, FIONBIO, &ul);
    // ép bàn phím thành non-blocking //
    ioctl(0, FIONBIO, &ul);

    char buf[256];

    while(1)
    {
        int bytes_recieved = recvfrom(udp_socket, buf, sizeof(buf) - 1, 0, NULL, NULL);
        if(bytes_recieved > 0)
        {
            printf("\r\033[K");
            buf[bytes_recieved] = '\0';
            printf("\r[Bạn kia]: %s\n", buf);
            printf("Bạn: ");
            fflush(stdout);

        }

        int bytes_read = read(0, buf, sizeof(buf) - 1);
        if(bytes_read > 0)
        {
            buf[bytes_read] = '\0';
            buf[strcspn(buf, "\n")] = 0;

            sendto(udp_socket, buf, strlen(buf), 0, (struct sockaddr*)&destAddr, sizeof(destAddr));

            printf("Bạn: ");
            fflush(stdout);
        }

        usleep(10000);


    }

}