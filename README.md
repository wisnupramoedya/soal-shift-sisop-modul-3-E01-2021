# soal-shift-sisop-modul-3-E01-2021

## Soal No.1

Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. 

Di dalam proyek itu, Keverk diminta:

### (a)
Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama akun.txt dengan format 
```
id:password
id2:password2
```

### (b)
Sistem memiliki sebuah database yang bernama files.tsv. Isi dari files.tsv ini adalah path file saat berada di server, publisher, dan tahun publikasi. Setiap penambahan dan penghapusan file pada folder file yang bernama  FILES pada server akan memengaruhi isi dari files.tsv. Folder FILES otomatis dibuat saat server dijalankan. 

### (c)
Tidak hanya itu, Keverk juga diminta membuat fitur agar client dapat menambah file baru ke dalam server. `Direktori FILES` memiliki struktur direktori di bawah ini : 
```
File1.ekstensi
File2.ekstensi
```
Pertama client mengirimkan input ke server dengan struktur sebagai berikut :

Contoh Command Client :
```
add
```
Output Client Console:
```
Publisher:
Tahun Publikasi:
Filepath:
```
Kemudian, dari aplikasi client akan dimasukan data buku tersebut (perlu diingat bahwa Filepath ini merupakan path file yang akan dikirim ke server). Lalu client nanti akan melakukan pengiriman file ke aplikasi server dengan menggunakan socket. Ketika file diterima di server, maka row dari files.tsv akan bertambah sesuai dengan data terbaru yang ditambahkan.
### (d)
Dan client dapat mendownload file yang telah ada dalam folder FILES di server, sehingga sistem harus dapat mengirim file ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut. 


Contoh Command client
`download TEMPfile.pdf`

### (e)
Setelah itu, client juga dapat menghapus file yang tersimpan di server. Akan tetapi, Keverk takut file yang dibuang adalah file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus.

Contoh Command Client: `delete TEMPfile.pdf`

### (f)
Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Output dari perintah tersebut keluar dengan format. 

Contoh Command Client :

`see`

Contoh Format Output pada Client:
```
Nama:
Publisher:
Tahun publishing:
Ekstensi File : 
Filepath : 

Nama:
Publisher:
Tahun publishing:
Ekstensi File : 
Filepath : 
```

### (g)
Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f.

Contoh Client Command:

`find TEMP`
### (h)
Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama running.log. Contoh isi dari log ini adalah

`running.log`
```
Tambah : File1.ektensi (id:pass)
Hapus : File2.ektensi (id:pass)
```

**Note :**
- Note: 
Dilarang menggunakan system() dan execv(). Silahkan dikerjakan sepenuhnya dengan thread dan socket programming. 
Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.

```
├── Client
│   ├── client.c
│   ├── File2.extensi
│   └── File1.extensi
└── Server
    ├── akun.txt
    ├── files.tsv
    ├── server.c
    ├── running.log
    └── FILES
            ├── File2.extensi
            └── File1.ekstensi
```
## Penyelesaian No. 1

### 1.a
Digunakan `auth` untuk mengklasifikasikan akun client. akun yang login saat ini akan disimpan di `user` untuk memudahkan log.
```
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
```
Jika berhasil `login`, maka akan lolos dari `while loop`

### 1.b
Diatur oleh variabel files. lalu dibuat prosedur penambahan ke files.tsv menggunakan `addtsv()`
```
FILE* files;
files = fopen(pathing(asal, "/files.tsv", hasil), "a+");
.
.
.
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
```

### 1.c
Command add akan menghasilkan info = 1. prosedur dipegang oleh bagian code dibawah.

`Client`
```
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
```

`Server`
```
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
```
Selebihnya dijelaskan saat demo.

### 1.d
Berikut adalah prosedur download. menggunakan mode `fopen` yang `rb` dan `wb`

`Client`

```
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
        }
```

`Server`
```
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
```
### 1.e
Dihandle oleh fungsi `deleteFile()` dan pada main ada juga.
```
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
}

.
.
.

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
```

### 1.f
DIhandle oleh fungsi `see()` dan di main ada juga.
```
else if(info == 3){
    see(files, check, display);
    send(new_socket, display, strlen(display), 0);
    continue;
}

.
.
.

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
```

### 1.g
DIhandle oleh fungsi `find()` dan di main ada juga. pertama harus di cek terlebih dahulu apakah ada di `files.tsv` nya.
```
else if(info == 4){
    sprintf(buffer, "Finding..");
    send(new_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
    read(new_socket, buffer, 1024);
    find(files, check, display, buffer);
    send(new_socket, display, strlen(display), 0);
    continue;
}

.
.
.

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
```

### 1.f
`running.log` telah dibuat diawal code. Dan disisipkan fungsi `logging()` pada saat command `add` dan `delete` dipanggil.
```
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
```

## Soal No.2
Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:
### 2.a 
Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
### 2.b 
Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (**Catatan!**: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) (**Catatan!**: gunakan thread untuk perhitungan di setiap cel). 
**Ketentuan**
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```

Contoh :

```
**A**	**B**	**Angka-Angka Faktorial**
4	4	4 3 2 1
4	5	4 3 2 1
4	3	4 3 2
4	0	0
0	4	0
4	6	4 3 2 1
```

Contoh :

```
**Matriks A**	**Matriks B**	**Matriks Hasil**
0	4	0	4	0	  4*3*2*1
4	5	6	2	4*3*2*1	  5*4
5	6	6	0	5*4*3*2*1 0
```
### 2.c
Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command ```“ps aux | sort -nrk 3,3 | head -5”``` (**Catatan!**: Harus menggunakan IPC Pipes)

Note:
- Semua matriks berasal dari input ke program.
- Dilarang menggunakan system()

## Penyelesaian No.2 :
Pada mulanya untuk setiap argumen akan diberikan alur thread tersendiri. Hal ini sesuai dengan ```argv[1]```. Dan untuk seluruh isi pemrograman thread mengambil dari modul 3.

### 2.a
DIdefinisikan beberapa variabel, R sebagai jumlah baris, C sebagai jumlah kolom dan RC adalah jumlah kolom/baris
```
#define R 4
#define RC 3
#define C 6
```

Mendeklarasikan matriks m1, m2, dan m_res sebagai matriks hasil perkalian
```
int m1[R][RC],
    m2[RC][C], 
    m_res[R*C];
```

Mendeklarasikan ```key_t``` sebagai kode unik yang akan berperan dalam shared memory ke dalam program ```soal2b.c``` dan array ```res``` sebagai array yang menampung hasil perkalian matriks
```
key_t key = 1945; 
	
int shmid = shmget(key, sizeof(int)*R*C, IPC_CREAT | 0666 );
int *res = (int *)shmat(shmid, NULL, 0);
```

Menginput nilai-nilai matriks ```m1``` dengan ukuran **4x3** dan input nilai-nilai matriks ```m2``` dengan ukuran **3x6**.
```
printf("Matrix %dx%d :\n", R, RC);
for(i=0; i < R; i++){
	for(j=0; j < RC; j++)
		scanf("%d",&m1[i][j] );
}

printf("\nMatrix %dx%d :\n", RC, C);
for(i=0; i < RC; i++){
	for(j=0; j < C; j++)
		scanf("%d",&m2[i][j] );
}
```

Deklarasi array thread dengan ukuran 24
```
pthread_t *tid=(pthread_t*)malloc((24)*sizeof(pthread_t));
```

Menyimpan elemen dari baris dan kolo ke dalam ```arg```
```
arg = (int*)malloc((24)*sizeof(int));
arg[0]=RC;

for (k=0; k<RC; k++){
	arg[k+1]=m1[i][k];
}
for (k=0; k<RC; k++){
	arg[k+RC+1]=m2[k][j];
}
```

Membuat thread
```
err = pthread_create(&(tid[count++]),NULL,&mul,(void*) arg);
if(err!=0)
	printf("\n can't create thread : [%s]",strerror(err));
```

Menjoin kan Thread dan ambil return value. Kemudian print hasil perkalian matriks
```
printf("\nHasil Perkalian :\n");
for(i=0; i < R*C; i++){
	void*k;
	pthread_join(tid[i],&k); 
	int* p = (int* )k;

	printf("%d ",*p);
	if((i+1)%C==0){
		printf("\n");
	}	
	m_res[i]=*p;
}
```	

Mmemasukkan nilai dari hasil perkalian ke dalam array ```res```
```
for(i=0; i < R*C; i++){
	res[i] = m_res[i];
}
```

## Soal No.3
Seorang mahasiswa bernama Alex sedang mengalami masa gabut. Di saat masa gabutnya, ia memikirkan untuk merapikan sejumlah file yang ada di laptopnya. Karena jumlah filenya terlalu banyak, Alex meminta saran ke Ayub. Ayub menyarankan untuk membuat sebuah program C agar file-file dapat dikategorikan. Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

Contoh apabila program dijalankan:
```
# Program soal3 terletak di /home/izone/soal3
$ ./soal3 -f path/to/file1.jpg path/to/file2.c path/to/file3.zip

# Hasilnya adalah sebagai berikut
/home/izone
|-jpg
    |--file1.jpg
|-c
    |--file2.c
|-zip
    |--file3.zip
```
### 3.a
Program menerima opsi -f seperti contoh di atas, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna. 
Output yang dikeluarkan adalah seperti ini :
```
File 1 : Berhasil Dikategorikan (jika berhasil)
File 2 : Sad, gagal :( (jika gagal)
File 3 : Berhasil Dikategorikan
```
### 3.b
Program juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin. Contohnya adalah seperti ini:
```
$ ./soal3 -d /path/to/directory/
```

Perintah di atas akan mengkategorikan file di /path/to/directory, lalu hasilnya akan disimpan di working directory dimana program C tersebut berjalan (hasil kategori filenya bukan di /path/to/directory).
Output yang dikeluarkan adalah seperti ini :
```
Jika berhasil, print “Direktori sukses disimpan!”
Jika gagal, print “Yah, gagal disimpan :(“
```

### 3.c
Selain menerima opsi-opsi di atas, program ini menerima opsi *, contohnya ada di bawah ini:
```
$ ./soal3 \*
```
Opsi ini akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut.

### 3.d
Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder “Hidden”.

### 3.e
Setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat.

Namun karena Ayub adalah orang yang hanya bisa memberi ide saja, tidak mau bantuin buat bikin programnya, Ayub meminta bantuanmu untuk membuatkan programnya. Bantulah agar program dapat berjalan!

Catatan: 
- Kategori folder tidak dibuat secara manual, harus melalui program C
- Program ini tidak case sensitive. Contoh: JPG dan jpg adalah sama
- Jika ekstensi lebih dari satu (contoh “.tar.gz”) maka akan masuk ke folder dengan titik terdepan (contoh “tar.gz”)
- Dilarang juga menggunakan fork-exec dan system()
- Bagian b dan c berlaku rekursif

## Penyelesaian No.3 :
Pada mulanya untuk setiap argumen akan diberikan alur thread tersendiri. Hal ini sesuai dengan ```argv[1]```. Dan untuk seluruh isi pemrograman thread mengambil dari modul 3.

### 3.a
Ini berlaku untuk ketika argumen awal adalah **-f**.
Di sini dialokasikan memori tid dan arg sesuai dengan jumlah banyaknya argumen setelah -f. Dalam kasus ini arg akan menyimpan nama file dan nomor file tersebut.
```
typedef struct thread_args_f {
    char *fileloc;
    int filenumber;
} args_f;
```

Dalam contoh ini, kita bisa mengurangi argc dengan 2 untuk mendapat banyak argumen tanpa nama file dan -f.
```
length = argc - 2;
tid = (pthread_t *)malloc(length * sizeof(pthread_t));
arg = (args_f *)malloc(length * sizeof(args_f));
```

Selanjutnya, untuk setiap argumen, dilakukan passing satu per satu dari ```argv[2]``` hingga akhir ke thread.
```
for (i = 0; i < length; i++) {
    arg[i].fileloc = argv[i + 2];
    arg[i].filenumber = i + 1;

    err = pthread_create(&(tid[i]), NULL, categorize_file, &arg[i]);
    if (err) printf("Error");
}
```

Di dalam fungsi categorize_file, sama dengan mengecek id dari thread dengan pthread_equal. Di dalamnya, dibuatkan beberapa variabel, yakni:
```
args_f *args = (args_f *)arg;
int check = -1;
char ext[20];
char filename[50];
char filepath[100];
```

Argumen arg yang tadi diambil, lalu dilakukan pemanggilan fileloc yang dipassing dengan beberapa variabel tadi untuk membagi susunan dari fileloc.
```
get_file_data(args->fileloc, ext, filename, filepath);
```

Di dalam get_file_data, kita memasukkan folder path awal dari file yang ingin dipindah ke dalam char filepath. Pada kasus ini, loop dilakukan dari belakang dan akan berhenti saat `fileloc[i]` sama dengan `'\'`.
```
for (i = strlen(fileloc); i >= 0; i--) {
    if (fileloc[i] == '/') {
        char temp = fileloc[i + 1];
        fileloc[i + 1] = '\0';
        // printf("i: %d\n", i);
        sprintf(filepath, "%s", fileloc);
        fileloc[i + 1] = temp;
        break;
    }
}
```
Setelahnya dilakukan passing nama file tadi ke char filename. Namun, kita tinggal memakai lokasi terakhir dari karakter `'\'`.
```
sprintf(filename, "%s", fileloc + i + 1);
```

Selain itu, untuk mendapatkan extensi dari file yang dimasukkan ke char ext. Untuk cara ini, kita mengambil lokasi terakhir, dan mencoba mendeteksi karakter titik yang pertama dari i terakhir tadi.
```
int save_i = ++i;
int status_ext = 0;

for (; i < strlen(fileloc); i++) {
    if (fileloc[i] == '.') {
        status_ext = 1;
        int j, k = i + 1;
        for (j = 0; fileloc[k];) {
            ext[j++] = tolower(fileloc[k++]);
        }
        break;
    }
}
```

Setelah semua data diberikan ke variabel pointer. Dilakukan pembuatan directory `ext` dengan mkdir. Selepasnya dibuatkan lokasi baru dengan format `/ext/namafile.ext`. Kita lantas pindahkan dengan fungsi rename. Tak lupa untuk mengecek apakah file berhasil dipindah atau tidak.
```
if (access(args->fileloc, F_OK) == 0) {
    mkdir(ext, 0777);

    char new_file_loc[100];
    sprintf(new_file_loc, "%s/%s", ext, filename);

    check = rename(args->fileloc, new_file_loc);
}
```

Selanjutnya, untuk menunggu proses print status setiap file, digunakan check yang sudah dipasang sebelumnya.
```
if (strcmp(argv[1], "-f") == 0) {
    for (i = 0; i < length; i++) {
        if (arg[i].isSuccess)
            printf("File %d : Berhasil Dikategorikan\n", arg[i].filenumber);
        else
            printf("File %d : Sad, gagal :(\n", arg[i].filenumber);
    }
}
```


### 3.b
Untuk soal ini, sama seperti sebelumnya namun dengan argumen **-d**. Kita memanggil categorize_path_dir yang mengambil lokasi path yang dilambangkan `argv[2]` dan pointer untuk menyimpan nama file dan nomor file.
```
categorize_path_dir((char *)argv[2], arg);
```

Di sini, digunakan konsep Queue untuk menampung seluruh data. Nantinya, queue tersebut akan menampung seluruh file di directory filedir secara rekursif.
```
Queue queue;
queue_init(&queue);

list_dir(filedir, &queue);
```

Di dalam list_dir, dilakukan pendataan nama file di dalam path secara rekursif hingga ke dalamnya. File path yang didapat lantas di-push ke queue.
```
void list_dir(char *name, Queue *queue) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name))) return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            list_dir(path, queue);
        } else {
            char fileloc[400];
            sprintf(fileloc, "%s/%s", name, entry->d_name);
            queue_push(queue, fileloc);
        }
    }
    closedir(dir);
}
```
Jika queue yang dipassing ternyata kosong maka tidak mungkin ada filepath yang ada sehingga tidak ada yang bisa disimpan.
```
if (queue_isEmpty(&queue)) {
    printf("Yah, gagal disimpan :(\n");
    exit(EXIT_FAILURE);
}
```

Selanjutnya, dilakukan pendataan length dari tid dan arg untuk dipesankan memory.
```
length = queue._size;s
tid = (pthread_t *)malloc(length * sizeof(pthread_t));
arg = (args_f *)malloc(length * sizeof(args_f));
```

Selanjutnya, sama seperti pada 3.a.
```
while (!queue_isEmpty(&queue)) {
    arg[i].fileloc = queue_front(&queue);
    arg[i].filenumber = i + 1;
    // printf("i:%d => fr: %s; fileloc: %s; filenumber: %d\n", i,
    //        queue._front->data, arg[i].fileloc, arg[i].filenumber);

    err = pthread_create(&(tid[i]), NULL, categorize_file, &arg[i]);
    if (err) printf("Error");
    queue_pop(&queue);
    i++;
}
```

Jika keseluruhan berjalan lancar, maka akan diprintkan
```
printf("Direktori sukses disimpan!\n");
```

### 3.c
Untuk kasus ini, sama dengan di atas namun memanggil `*`. Dilakukan pemanggilan ke categorize_work_dir.
```
categorize_work_dir(arg);
```

Di sini dilakukan pemanggilan pwd dengan
```
if (getcwd(pwd, sizeof(pwd)) == NULL) exit(EXIT_FAILURE);
```

Selanjutnya, kita dapat memasing lokasi pwd dengan fungsi pada 3.b.
```
categorize_path_dir(pwd, arg);
```

### 3.d
Di sini, kita perlu melakukan antisipasi terhadap beberapa kasus file tersembunyi dan file tanpa ekstensi.

Untuk tersembunyi, kita tinggal mengecek apakah pada namafile terdapat tanda `.` di depannya.
```
if (save_i == i && fileloc[save_i] == '.') {
    status = HIDDEN_FILE;
    continue;
}
```

Untuk tidak diketahui ektensi, kita tinggal menambahkan status apakah terdapat titik pada pertengahan ekstensi. Jika ternyata tidak terdapat status_ext yang berubah, maka file tidak memiliki ekstensi.
```
if (!status_ext) {
    status = UNKNOWN_FILE;
}
```

Terakhir, kita tinggal mengganti ext sesuai dengan status yang didapat tadi.
if (status == UNKNOWN_FILE) {
    sprintf(ext, "Unknown");
} else if (status == HIDDEN_FILE) {
    sprintf(ext, "Hidden");
}

### 3.e
Untuk ini, file sudah berjalan secara parallel dengan adanya thread pada categorize_file.
```
void *categorize_file(void *arg) {
    pthread_t id = pthread_self();
    int i;

    for (i = 0; i < length; i++) {
        if (pthread_equal(id, tid[i])) {
            // kode program
        }
    }
}
```

Setiap akhir thread akan dilakukan pthread_join terhadap file.
```
for (i = 0; i < length; i++) pthread_join(tid[i], NULL);
```

## Kendala
- Script perlu penghalang, karena dapat merusak susunan directory
- Terdapat ETS yang perhatian jauh lebih utama
