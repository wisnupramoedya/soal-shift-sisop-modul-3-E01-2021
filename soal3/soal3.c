#include <ctype.h>
#include <dirent.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int EXT_FILE = 0, UNKNOWN_FILE = 1, HIDDEN_FILE = 2;
pthread_t *tid;
int length = 0;
char pwd[200];

typedef struct queueNode_t {
    char data[200];
    int number;
    struct queueNode_t *next;
} QueueNode;

/* Struktur ADT Queue */

typedef struct queue_t {
    QueueNode *_front, *_rear;
    unsigned _size;
} Queue;

/* Function prototype */

void queue_init(Queue *queue);
bool queue_isEmpty(Queue *queue);
void queue_push(Queue *queue, char *value);
void queue_pop(Queue *queue);
char *queue_front(Queue *queue);
int queue_size(Queue *queue);

/* Function definition below */

void queue_init(Queue *queue) {
    queue->_size = 0;
    queue->_front = NULL;
    queue->_rear = NULL;
}

bool queue_isEmpty(Queue *queue) {
    return (queue->_front == NULL && queue->_rear == NULL);
}

void queue_push(Queue *queue, char *value) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (newNode) {
        queue->_size++;
        strcpy(newNode->data, value);
        newNode->next = NULL;

        if (queue_isEmpty(queue))
            queue->_front = queue->_rear = newNode;
        else {
            queue->_rear->next = newNode;
            queue->_rear = newNode;
        }
    }
}

void queue_pop(Queue *queue) {
    if (!queue_isEmpty(queue)) {
        QueueNode *temp = queue->_front;
        queue->_front = queue->_front->next;
        // free(temp);

        if (queue->_front == NULL) queue->_rear = NULL;
        queue->_size--;
    }
}

char *queue_front(Queue *queue) {
    if (!queue_isEmpty(queue)) {
        return (queue->_front->data);
    }
    return NULL;
}

int queue_size(Queue *queue) { return queue->_size; }

typedef struct thread_args_f {
    char *fileloc;
    int filenumber;
} args_f;

void get_file_data(char *fileloc, char *ext, char *filename, char *filepath) {
    int i, status = 0;

    // to get filepath
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

    // to get filename
    sprintf(filename, "%s", fileloc + i + 1);

    // to split filename
    int save_i = ++i;
    int status_ext = 0;
    // printf("i: %d save_i: %d\n", i, save_i);
    for (; i < strlen(fileloc); i++) {
        // to solve hidden file
        if (save_i == i && fileloc[save_i] == '.') {
            status = HIDDEN_FILE;
            continue;
        }
        if (fileloc[i] == '.') {
            status_ext = 1;
            int j, k = i + 1;
            for (j = 0; fileloc[k];) {
                ext[j++] = tolower(fileloc[k++]);
            }
            break;
        }
    }
    // printf("%s", ext);

    if (!status_ext) {
        status = UNKNOWN_FILE;
    }

    if (status == UNKNOWN_FILE) {
        sprintf(ext, "Unknown");
    } else if (status == HIDDEN_FILE) {
        sprintf(ext, "Hidden");
    }
}

void *categorize_file(void *arg) {
    pthread_t id = pthread_self();
    int i;

    for (i = 0; i < length; i++) {
        if (pthread_equal(id, tid[i])) {
            args_f *args = (args_f *)arg;
            // printf("%s %d\n", args->fileloc, args->filenumber);
            int check = -1;
            char ext[20];
            char filename[50];
            char filepath[100];

            get_file_data(args->fileloc, ext, filename, filepath);

            // printf(
            //     "ext: %s; filename: %s; foldername: %s; "
            //     "folderpath: %s\n",
            //     ext, filename, filepath, args->fileloc);

            if (access(args->fileloc, F_OK) == 0) {
                mkdir(ext, 0777);

                char new_file_loc[100];
                sprintf(new_file_loc, "%s/%s", ext, filename);
                // printf("%s\n", new_file_loc);
                check = rename(args->fileloc, new_file_loc);
            }

            // while (mutex_status != i)
            //     ;
            if (check == 0)
                printf("File %d : Berhasil Dikategorikan\n", args->filenumber);
            else
                printf("File %d : Sad, gagal :(\n", args->filenumber);
            // mutex_status = 1;
        }
    }
}

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
            // printf("%s\n", queue->_rear->data);
        }
    }
    closedir(dir);
}

void categorize_path_dir(char *filedir, args_f *arg) {
    Queue queue;
    queue_init(&queue);

    list_dir(filedir, &queue);

    if (queue_isEmpty(&queue)) {
        printf("Yah, gagal disimpan :(\n");
        exit(EXIT_FAILURE);
    }

    int i = 0, err;
    length = queue._size;
    // printf("l: %d\n", length);
    tid = (pthread_t *)malloc(length * sizeof(pthread_t));
    arg = (args_f *)malloc(length * sizeof(args_f));

    // printf("s : %d\n", queue._size);
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

    printf("Direktori sukses disimpan!\n");
}

void *categorize_work_dir(args_f *arg) {
    if (getcwd(pwd, sizeof(pwd)) == NULL) exit(EXIT_FAILURE);

    categorize_path_dir(pwd, arg);
}

int main(int argc, char *argv[]) {
    // Queue queue;
    int err, i;
    args_f *arg;

    // printf("%s", argv[1]);
    if (strcmp(argv[1], "-f") == 0) {
        length = argc - 2;
        tid = (pthread_t *)malloc(length * sizeof(pthread_t));
        arg = (args_f *)malloc(length * sizeof(args_f));

        // printf("-f %d %s\n", length, argv[2]);
        for (i = 0; i < length; i++) {
            arg[i].fileloc = argv[i + 2];
            arg[i].filenumber = i + 1;

            err = pthread_create(&(tid[i]), NULL, categorize_file, &arg[i]);
            if (err) printf("Error");
        }

    } else if (strcmp(argv[1], "-d") == 0) {
        // printf("-d\n");
        categorize_path_dir((char *)argv[2], arg);
    } else if (strcmp(argv[1], "*") == 0) {
        // printf("*\n");
        categorize_work_dir(arg);
    }

    for (i = 0; i < length; i++) pthread_join(tid[i], NULL);
    return 0;
}
