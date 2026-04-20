#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


int main() {
    struct addrinfo *res, *p;
    int ret = getaddrinfo(argv[1], "http", NULL, &res);
    if(ret != 0)
    {
        printf("GET IP UNSUCCESSFULLY!");
        return 1;
    }
    p = res;
    while(p != NULL)
    {
        if(p->ai_addr == AF_INET)
        {
            printf("IPv4\n");
            struct sockaddr_in addr;
            memccpy(&addr, p->ai_addr, p->ai_addrlen);
            printf("IP: %s\n", inet_ntoa(addr.sin_addr));
        }
        else if(p->ai_family == AF_INET6)
        {
            printf("IPv6\n");
            char buf[64];
            struct sockaddr_in6 addr6;
            memcpy(&addr6, p->ai_addr, p->ai_addrlen);
            printf("IP: %s\n", inet_ntop(p->ai_addr, &addr6.sin6_addr, buf, sizeof(addr6)));
        }
        p = p->ai_next;

    }

}