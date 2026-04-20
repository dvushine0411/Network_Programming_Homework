#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h> 

struct ClientState {
    int fd;
    int state; 
    char name[256];
    char mssv[64];
};

void generateEmail(char *name, char *mssv, char *result) {

    name[strcspn(name, "\r\n")] = '\0';
    mssv[strcspn(mssv, "\r\n")] = '\0';

    // Trích ra tên của sinh viên // 
    char mainEmailName[64] = {0};
    char *lastSpace = strrchr(name, ' ');

    if(lastSpace != NULL)
    {
        char *lastWord = lastSpace + 1;
        strcpy(mainEmailName, lastWord);
    }

    mainEmailName[0] = toupper(mainEmailName[0]);

    // Trích ra chữ hoa của từng chữ đầu trong tên sinh viên //

    char middleName[5] = {0};
    middleName[0] = toupper(name[0]);
    int j = 1;
    int lenName = strlen(name);
    for(int i = 0; i < lenName; i++)
    {
        if(&name[i] == lastSpace)
        {
            break;
        }
        else if(name[i] == ' ')
        {
            middleName[j] = toupper(name[i + 1]);
            j++;
        }
    }

    // Trích ra mã số sinh viên trong email của sinh viên //

    char mainEmailMSSV[64] = {0};
    int lenMssv = strlen(mssv);
    int num = 0;

    for(int i = 2; i < lenMssv; i++)
    {
        mainEmailMSSV[num] = mssv[i];
        num++;
    }


    sprintf(result, "%s.%s%s@sis.hust.edu.vn", mainEmailName, middleName, mainEmailMSSV);

}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listener == -1) {
        perror("socket() failed");
        return 1;
    }

    unsigned long ul = 1;
    ioctl(listener, FIONBIO, &ul);

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
        perror("setsockopt() failed");
        close(listener);
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);

    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        close(listener);
        return 1;
    }

    if(listen(listener, 5)) {
        perror("listen() failed");
        close(listener);
        return 1;
    }

    printf("Server is now listening on port 8080...\n");

    struct ClientState clients[64];
    int nClients = 0;
    char buf[256];

    while(1) {
        int new_client = accept(listener, NULL, NULL);
        if(new_client != -1) {
            printf("New client connected: FD %d\n", new_client);
            
            ioctl(new_client, FIONBIO, &ul);
            
            if (nClients < 64) {

                clients[nClients].fd = new_client;
                clients[nClients].state = 0; // Trạng thái bắt đầu là 0 (chờ Name)
                memset(clients[nClients].name, 0, sizeof(clients[nClients].name));
                memset(clients[nClients].mssv, 0, sizeof(clients[nClients].mssv));
                nClients++;
                
                char *msg = "Vui long nhap Ho va Ten: ";
                send(new_client, msg, strlen(msg), 0);
            } else {
                printf("Server full. Tu choi client %d.\n", new_client);
                close(new_client);
            }
        }

        for (int i = 0; i < nClients; i++) {
            memset(buf, 0, sizeof(buf));
            int ret = recv(clients[i].fd, buf, sizeof(buf) - 1, 0);
            
            if (ret > 0) {
                buf[ret] = '\0'; 
                
                if (clients[i].state == 0) {

                    strcpy(clients[i].name, buf);
                    clients[i].state = 1; 
                    
                    char *msg = "Vui long nhap MSSV: ";
                    send(clients[i].fd, msg, strlen(msg), 0);
                    
                } else if (clients[i].state == 1) {
                    strcpy(clients[i].mssv, buf);
                    
                    char email[512];
                    generateEmail(clients[i].name, clients[i].mssv, email);
                    
                    char response[1024];
                    sprintf(response, "Email sinh vien cua ban la: %s\n", email);
                    send(clients[i].fd, response, strlen(response), 0);
                    
                    printf("Da tao email cho FD %d: %s\n", clients[i].fd, email);
                    
                    close(clients[i].fd);
                
                    clients[i] = clients[nClients - 1];
                    nClients--;
                    i--; // 
                }
            } else if (ret == 0) {
                printf("Client FD %d disconnected.\n", clients[i].fd);
                close(clients[i].fd);
                clients[i] = clients[nClients - 1];
                nClients--;
                i--;
            } else {
                if (errno != EWOULDBLOCK && errno != EAGAIN) {

                    close(clients[i].fd);
                    clients[i] = clients[nClients - 1];
                    nClients--;
                }
            }
        }
    }
}