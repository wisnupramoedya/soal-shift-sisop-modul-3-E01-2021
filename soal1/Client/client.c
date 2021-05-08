#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define SIZE 1024
 
int createClientSocket(struct sockaddr_in *address, struct sockaddr_in *serv_addr);
char *pathing(char path1[], char path2[], char hasil[]);
 
int main(int argc, char const *argv[])
{
    struct sockaddr_in address, serv_addr;
    char buffer[1024];

    char asal[100] = "/home/nabil/Documents/Modul3/Client";
    char hasil[100];
 
    int client_fd = createClientSocket(&address, &serv_addr);
 
    while (1) {
        scanf("%s", buffer);
        if(strcmp(buffer,"add")==0){
            send(client_fd, buffer, strlen(buffer), 0); //add
            memset(buffer, 0, sizeof(buffer));
            read(client_fd, buffer, 1024);
            printf("%s\n", buffer);
            scanf("%s", buffer);
            send(client_fd, buffer, strlen(buffer), 0); //publisher
            memset(buffer, 0, sizeof(buffer));
            read(client_fd, buffer, 1024);
            printf("%s\n", buffer);
            scanf("%s", buffer);
            send(client_fd, buffer, strlen(buffer), 0); //tahun
            memset(buffer, 0, sizeof(buffer));
            read(client_fd, buffer, 1024);
            printf("%s\n", buffer);
            scanf("%s", buffer);
            send(client_fd, buffer, strlen(buffer), 0); //filepath
            strcpy(hasil,buffer);
            memset(buffer, 0, sizeof(buffer));
            read(client_fd, buffer, 1024);
            printf("%s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
            // sleep(5);
            FILE *fp;
            fp = fopen(hasil,"rb");
            if (fp == NULL) {
                perror("[-]Error in reading file.");
                exit(1);
            }
            char data[SIZE] = {0};
            int n;
            do{
                n = fread(data, 1,1024, fp);
                send(client_fd, data, sizeof(data), 0);
                // puts(",,,");
            } while(n == sizeof(data));
            memset(buffer, 0, sizeof(buffer));
            fclose(fp);
            send(client_fd, "END", sizeof("END"), 0);
            continue;
        }
        else if(strcmp(buffer,"download")==0){
            send(client_fd, buffer, strlen(buffer), 0); //download
            memset(buffer, 0, sizeof(buffer));
            read(client_fd, buffer, 1024);
            printf("%s\n", buffer);
            scanf("%s", buffer);
            send(client_fd, buffer, strlen(buffer), 0); //file
            char data[1024];
            FILE *rcv;
            sprintf(hasil,"%s/%s",asal,buffer);
            rcv = fopen(hasil,"wb");
            int n;
            while (1) {
                n = recv(client_fd, data, 1024, 0);
                if (strcmp(data,"END")==0){
                break;
                }
                fwrite(data,1,sizeof(data),rcv);
                // puts("...");
                bzero(data, 1024);
            }
            fclose(rcv);
            continue;
        } else if(strcmp(buffer,"exit")==0){
            send(client_fd, buffer, strlen(buffer), 0);
            break;
        }
        send(client_fd, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, 1024);
        printf("%s\n", buffer);
    }
    return 0;
}


char *pathing(char path1[], char path2[], char hasil[]){
    strcpy(hasil, path1);
    strcat(hasil, path2);
    return hasil;
}
 
int createClientSocket(struct sockaddr_in *address, struct sockaddr_in *serv_addr)
{
    int fd;
 
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    memset(serv_addr, '0', sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
 
    if (inet_pton(AF_INET, "127.0.0.1", &(serv_addr->sin_addr)) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(fd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return fd;
}