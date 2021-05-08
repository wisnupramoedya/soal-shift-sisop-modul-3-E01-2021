#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PORT 8080
#define SIZE 1024

char user[1024];
 
int createServerSocket(struct sockaddr_in *address, int *addrlen);
int command(char buffer[], int *flag);
char *pathing(char path1[], char path2[], char hasil[]);
int lr(char buffer[], int *flag);
void addtsv(FILE *files, char nama[], char publisher[], char tahun[], char ekstensi[], char path[]);
int deletefile(FILE *files, char name[], char asal[], char hasil[]);
void see(FILE *files, char check[], char display[]);
void find(FILE *files, char check[], char display[], char buffer[]);
int locate(char fullpath[], char sy);
void logging(FILE* log,int tipe, char nama[]);
 
int main()
{
    struct sockaddr_in address;
    int new_socket, addrlen, flag = 1, info, auth = 0;
    char delim[] = ":";
    char buffer[1024];
    char check[1024];
    char asal[100] = "/home/nabil/Documents/Modul3/Server";
    char hasil[100];
    char display[10000];


    FILE* akun;
    FILE* files;
    FILE* log;
    akun = fopen(pathing(asal, "/akun.txt", hasil), "a+");
    files = fopen(pathing(asal, "/files.tsv", hasil), "a+");
    log = fopen(pathing(asal, "/log.tsv", hasil), "a+");
    int status = mkdir(pathing(asal, "/FILES", hasil),0777); //folder FILES
 
    int server_fd = createServerSocket(&address, &addrlen);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
 
    while (flag) {
        while(flag == 1 && auth == 0){
            fclose(akun);
            akun = fopen(pathing(asal, "/akun.txt", hasil), "a+");
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            info = lr(buffer, &flag);
            send(new_socket, buffer, strlen(buffer), 0);
            if(info == 1) { //login
                char login[1024];
                char pass[1024];
                memset(buffer, 0, sizeof(buffer)); //id
                read(new_socket, buffer, 1024);
                strcpy(login, buffer);
                send(new_socket, "Expecting pass...", strlen("Expecting pass..."), 0);
                memset(buffer, 0, sizeof(buffer)); // pass
                read(new_socket, buffer, 1024);
                strcpy(pass, buffer);
                while(fgets(check, sizeof(check), akun))
                {
                    char *ptr = strtok(check,delim);
                    if (strcmp(ptr, login) == 0) 
                    {
                        ptr = strtok(NULL,delim);
                        ptr[strcspn(ptr,"\n")]='\0';
                        // printf("%d\n",strcmp(ptr, pass));
                        if (strcmp(ptr, pass) == 0) 
                        {
                            sprintf(user,"%s:%s", login, pass);
                            auth=1;
                            break;
                        }
                    }
                }
                if(auth == 1)
                send(new_socket, "Login berhasil", strlen("Login berhasil"), 0);
                else send(new_socket, "Login gagal", strlen("Login gagal"), 0);
            } else if(info == 2){ //register
                int pesan = 0;
                char login[1024];
                char pass[1024];
                memset(buffer, 0, sizeof(buffer)); //id
                read(new_socket, buffer, 1024);
                strcpy(login, buffer);
                send(new_socket, "Expecting pass...", strlen("Expecting pass..."), 0);
                memset(buffer, 0, sizeof(buffer)); // pass
                read(new_socket, buffer, 1024);
                strcpy(pass, buffer);
                
                while(fgets(check, sizeof(check), akun))
                {
                    char *ptr = strtok(check,delim);
                    puts(ptr);
                    puts(login);
                    if (strcmp(ptr, login) == 0) 
                    {
                        pesan = 1;
                        break;
                    }
                }
                if(pesan == 0){
                    fputs(login, akun);
                    fputs(":", akun);
                    fputs(pass, akun);
                    fputs("\n", akun);
                }

                if(pesan == 0)
                send(new_socket, "Berhasil mendaftar.", strlen("Berhasil mendaftar."), 0);
                else send(new_socket, "Gagal mendaftar.", strlen("Gagal mendaftar."), 0);
            }
        }
        fclose(files);
        fclose(log);
        files = fopen(pathing(asal, "/files.tsv", hasil), "a+");
        log = fopen(pathing(asal, "/log.tsv", hasil), "a+");
        memset(display, 0, strlen(display));
        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, 1024);
        info = command(buffer, &flag);
        if(info == 1){
            char comp[1024];
            char nama[1024];
            char publish[1024];
            char tahun[1024];
            char eks[1024];
            char pathnya[1024];
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            strcpy(publish,buffer);
            send(new_socket, "Tahun publikasi:", strlen("Tahun publikasi:"), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            strcpy(tahun,buffer);
            send(new_socket, "Filepath:", strlen("Filepath:"), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            send(new_socket, "Transferring..", strlen("Transferring.."), 0);
            strcpy(comp,buffer);
            char *ptr;
            ptr = comp + locate(comp,'/') + 1;
            strcpy(nama,ptr);
            logging(log,1,nama);
            strcpy(comp,nama);
            ptr = comp + locate(comp,'.') + 1;
            strcpy(eks,ptr);
            char *p = strtok(comp,".");
            strcpy(nama,p);
            sprintf(display,"%s/FILES/%s.%s",asal,nama,eks);
            char data[1024];
            FILE *rcv;
            rcv = fopen(display,"wb");
            int n;
            while (1) {
                n = recv(new_socket, data, 1024, 0);
                if (strcmp(data,"END")==0){
                break;
                }
                fwrite(data,1,sizeof(data),rcv);
                // puts("...");
                bzero(data, 1024);
            }
            fclose(rcv);
            addtsv(files,nama,publish,tahun,eks,display);
            continue;
        } else if(info == 2){
            sprintf(buffer, "Processing..");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            if(deletefile(files,buffer,asal,hasil) == 1)
            logging(log,2,buffer);
            fclose(files);
            remove(pathing(asal, "/files.tsv", hasil));

            char hasil2[100];
            strcpy(hasil2,pathing(asal, "/files.tsv", hasil));
            rename(pathing(asal, "/temp.tsv", hasil), hasil2);

            files = fopen(pathing(asal, "/files.tsv", hasil), "a+");
            sprintf(buffer, "Processed..");
        } else if(info == 3){
            see(files, check, display);
            send(new_socket, display, strlen(display), 0);
            continue;
        } else if(info == 4){
            sprintf(buffer, "Finding..");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            find(files, check, display, buffer);
            send(new_socket, display, strlen(display), 0);
            continue;
        } else if(info == 5){
            int muncul =0;
            sprintf(buffer, "Downloading..");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));
            read(new_socket, buffer, 1024);
            sprintf(display,"%s/FILES/%s",asal,buffer);
            char *p;
            char na[1024],nana[1024], eks[1024];
            strcpy(na,buffer);
            p = buffer + locate(buffer,'.');
            strcpy(eks,p);
            char *pna = strtok(na,".");
            strcpy(nana,pna);
            while(fgets(check, 1024 , files)) //check validity
            {
                int i = 0;
                char comp[1024];
                
                strcpy(comp,check);
                
                char iter[5][1024];
                char *ptr = strtok(comp,"\t");

                while( ptr != NULL){
                    strcpy(iter[i++],ptr);
                    ptr = strtok(NULL,"\t");
                }
                
                if (strcmp(iter[0], nana) == 0 && strcmp(iter[3], eks) == 0) 
                {
                    muncul++;
                }
            }
            fclose(files);
            files = fopen(pathing(asal, "/files.tsv", hasil), "a+");
            if(muncul == 0) send(new_socket, "END", sizeof("END"), 0);
            if(muncul > 0){
                FILE *fp;
                fp = fopen(display,"rb");
                if (fp == NULL) {
                    perror("[-]Error in reading file.");
                    exit(1);
                }
                char data[SIZE] = {0};
                int n;
                do{
                    n = fread(data, 1,1024, fp);
                    send(new_socket, data, sizeof(data), 0);
                    // puts(",,,");
                } while(n == sizeof(data));
                memset(data, 0, sizeof(data));
                fclose(fp);
                send(new_socket, "END", sizeof("END"), 0);
            } else {
                sprintf(buffer, "File tidak valid, mohon dicek kembali");
            }
            continue;
        }
        send(new_socket, buffer, strlen(buffer), 0);
        // fclose(files);
        // files = fopen(pathing(asal, "/files.tsv", hasil), "a+");
    }

    fclose(akun);
    fclose(files);
    return 0;
}

void logging(FILE* log,int tipe, char nama[]){
    char hasil[1024];
    if(tipe == 1){
        sprintf(hasil,"Tambah : %s (%s)\n",nama,user);
        fputs(hasil,log);
    } else if(tipe == 2){
        sprintf(hasil,"Hapus : %s (%s)\n",nama,user);
        fputs(hasil,log);
    }
}

int locate(char fullpath[], char sy){
    int len = strlen(fullpath);
    while(--len){
        if(fullpath[len] == sy) break;
    }
    return len;
}

void find(FILE *files, char check[], char display[], char buffer[]){
    int muncul = 0;
    while(fgets(check, 1024 , files)!= NULL)
    {
        if(feof(files)){
            sprintf(display + strlen(display),"files.tsv kosong");
            return;
        }
        int i = 0;
        char comp[1024];
        strcpy(comp,check);
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");
        if(strcmp(buffer, ptr) == 0){
            muncul++;
        }
    }
    fclose(files);
    files = fopen(pathing("/home/nabil/Documents/Modul3/Server", "/files.tsv", check), "a+");
    if(muncul > 0)
    while(fgets(check, 1024 , files))
    {
        int i = 0;
        char comp[1024];
        strcpy(comp,check);
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");
        if(strcmp(buffer, ptr) != 0) continue;

        while( ptr != NULL){
            strcpy(iter[i++],ptr);
            ptr = strtok(NULL,"\t");
        }
        
        sprintf(display + strlen(display),"\nNama:%s\n",iter[0]);
        sprintf(display + strlen(display),"Publisher:%s\n",iter[1]);
        sprintf(display + strlen(display),"Tahun publishing:%s\n",iter[2]);
        sprintf(display + strlen(display),"Ekstensi File:%s\n",iter[3]);
        sprintf(display + strlen(display),"Filepath:%s",iter[4]);
    }
    else sprintf(display + strlen(display),"Nama file tidak ditemukan");
}

void see(FILE *files, char check[], char display[]){
    int muncul = 0;
    while(fgets(check, 1024 , files) != NULL)
    {
        int i = 0;
        char comp[1024];
        strcpy(comp,check);
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");

        while( ptr != NULL){
            strcpy(iter[i++],ptr);
            ptr = strtok(NULL,"\t");
        }
        muncul++;
        sprintf(display + strlen(display),"\nNama:%s\n",iter[0]);
        sprintf(display + strlen(display),"Publisher:%s\n",iter[1]);
        sprintf(display + strlen(display),"Tahun publishing:%s\n",iter[2]);
        sprintf(display + strlen(display),"Ekstensi File:%s\n",iter[3]);
        sprintf(display + strlen(display),"Filepath:%s",iter[4]);
    }
    if(muncul == 0) sprintf(display + strlen(display),"Nama file tidak ditemukan");
}

int deletefile(FILE *files, char name[], char asal[], char hasil[]){
    int flag=0;
    char check[1024];
    FILE *temp;
    temp = fopen(pathing(asal, "/temp.tsv", hasil), "a+");
    char *p;
    char na[1024],nana[1024], eks[1024];
    strcpy(na,name);
    p = name + locate(name,'.') +1;
    strcpy(eks,p);
    char *pna = strtok(na,".");
    strcpy(nana,pna);
    // puts(nana);
    //     puts(eks);
    while(fgets(check, sizeof(check), files) != NULL)
    {
        int i = 0;
        char comp[1024];
        
        strcpy(comp,check);
        
        char iter[5][1024];
        char *ptr = strtok(comp,"\t");

        while( ptr != NULL){
            strcpy(iter[i++],ptr);
            ptr = strtok(NULL,"\t");
        }
        
        if (strcmp(iter[0], nana) == 0 && strcmp(iter[3], eks) == 0) 
        {
            flag=1;
            continue;
        }
        fputs(check, temp);
    }
    fclose(temp);

    char hasil3[100];
    strcpy(hasil3,pathing(pathing(pathing(asal, "/FILES/", hasil), "old-", hasil),name,hasil));
    rename( pathing(pathing(asal, "/FILES/", hasil), name, hasil), hasil3);

    return flag;
}

void addtsv(FILE *files, char nama[], char publisher[], char tahun[], char ekstensi[], char path[]){
    fputs(nama, files);
    fputc('\t', files);
    fputs(publisher, files);
    fputc('\t', files);
    fputs(tahun, files);
    fputc('\t', files);
    fputs(ekstensi, files);
    fputc('\t', files);
    fputs(path, files);
    fputc('\n', files);
}

char *pathing(char path1[], char path2[], char hasil[]){
    strcpy(hasil, path1);
    strcat(hasil, path2);
    return hasil;
}

int lr(char buffer[], int *flag)
{
    if (strcmp(buffer, "login") == 0) {
        sprintf(buffer, "Expecting id...");
        return 1;
    }
    else if (strcmp(buffer, "register") == 0) {
        sprintf(buffer, "Expecting id...");
        return 2;
    } else if(strcmp(buffer, "exit") == 0){
        (*flag) = 0;
        sprintf(buffer, "Exiting..");
        return 3;
    }
    return 0;
}
 
int command(char buffer[], int *flag)
{
    if (strcmp(buffer, "add") == 0) {
        sprintf(buffer, "Publisher:");
        return 1;
    } else if(strcmp(buffer, "delete") == 0){
        return 2;
    } else if(strcmp(buffer, "see") == 0){
        return 3;
    } else if(strcmp(buffer, "find") == 0){
        return 4;
    } else if(strcmp(buffer, "download") == 0){
        return 5;
    } else if(strcmp(buffer, "exit") == 0){
        (*flag) = 0;
        sprintf(buffer, "Exiting..");
        return 0;
    }
    return 0;
}
 
int createServerSocket(struct sockaddr_in *address, int *addrlen)
{
    int fd, opt = 1;
 
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
 
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    *addrlen = sizeof(*address);
 
    if (bind(fd, (struct sockaddr *)address, *addrlen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return fd;
}