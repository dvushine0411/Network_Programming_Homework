#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>



int main() {
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);



    connect(client, (struct sockaddr *)&addr, sizeof(addr));


    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        int path_len = strlen(cwd);
        send(client, cwd, path_len, 0);
        send(client, "\n", 1, 0);
        
    }

    DIR *d = opendir(".");
    struct dirent *dir;
    struct stat st;


    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") == 0)   continue;
            if(stat(dir->d_name, &st) == 0)
            {
                char buffer[512];
                int len = sprintf(buffer, "%s - %ld bytes\n", dir->d_name, st.st_size);
                send(client, buffer, len, 0);
            }
        }
        closedir(d);

    }
    close(client);

}

