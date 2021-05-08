# soal-shift-sisop-modul-3-E01-2021

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
if (check == 0)
    printf("File %d : Berhasil Dikategorikan\n", args->filenumber);
else
    printf("File %d : Sad, gagal :(\n", args->filenumber);
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