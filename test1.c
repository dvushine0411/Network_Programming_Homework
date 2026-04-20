// Đoạn code phân giải tên miền //
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    struct addrinfo *res, *p;
    int ret = getaddrinfo(argv[1], "http", NULL, &res);
    if(ret != 0)
    {
        printf("Không thể phân giải tên miền này\n");
        exit(1);
    }
    else
    {
        p = res;
        while(p != NULL)
        {
            if(p->ai_family == AF_INET)
            {
                printf("IPv4:\n");
                struct sockaddr_in addr;
                memcpy(&addr, p->ai_addr, p->ai_addrlen);
                char buf[100];
                inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
                printf("IPv4: %s\n", buf);
            }
            else if(p->ai_family == AF_INET6)
            {
                printf("IPv6:\n");
                struct sockaddr_in6 addr;
                char buf[256];
                memcpy(&addr, p->ai_addr, p->ai_addrlen);
                inet_ntop(AF_INET6, &addr.sin6_addr, buf, sizeof(buf));
                printf("IPv6: %s\n", buf);
            }
            p = p->ai_next;
        }
    }

    freeaddrinfo(res);
    
}