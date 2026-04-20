#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) return 1;

    int port = atoi(argv[1]);
    char *f_chao = argv[2];
    char *f_ghi = argv[3];

    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) == -1) return 1;
    if (listen(listener, 5) == -1) return 1;

    while (1) {
        int client = accept(listener, NULL, NULL);
        if (client == -1) continue;

        FILE *f1 = fopen(f_chao, "r");
        if (f1 != NULL) {
            char buf[1024];
            int n = fread(buf, 1, sizeof(buf), f1);
            send(client, buf, n, 0);
            fclose(f1);
        }

        FILE *f2 = fopen(f_ghi, "a");
        if (f2 != NULL) {
            char buf[1024];
            int n;
            while ((n = recv(client, buf, sizeof(buf), 0)) > 0) {
                fwrite(buf, 1, n, f2);
                fflush(f2);
            }
            fclose(f2);
        }
        close(client);
    }
    close(listener);
    return 0;
}