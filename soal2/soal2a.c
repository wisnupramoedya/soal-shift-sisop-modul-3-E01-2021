#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define R 4
#define RC 3
#define C 6

void* mul(void* argvar) {
    int* arg = (int*)argvar;
    int i, n = 0;

    for (i = 1; i <= RC; i++) n += arg[i] * arg[i + RC];

    int* p = (int*)malloc(sizeof(int));
    *p = n;
    pthread_exit(p);
}

int m1[R][RC], m2[RC][C], m_res[R * C];

int main() {
    key_t key = 1945;

    int shmid = shmget(key, sizeof(int) * R * C, IPC_CREAT | 0666);
    int* res = (int*)shmat(shmid, NULL, 0);

    int i, j;
    printf("Matrix %dx%d :\n", R, RC);
    for (i = 0; i < R; i++) {
        for (j = 0; j < RC; j++) scanf("%d", &m1[i][j]);
    }

    printf("\nMatrix %dx%d :\n", RC, C);
    for (i = 0; i < RC; i++) {
        for (j = 0; j < C; j++) scanf("%d", &m2[i][j]);
    }

    int err, k;
    pthread_t* tid = (pthread_t*)malloc((24) * sizeof(pthread_t));

    int count = 0;
    int* arg = NULL;

    for (i = 0; i < R; i++) {
        for (j = 0; j < C; j++) {
            arg = (int*)malloc((24) * sizeof(int));
            arg[0] = RC;

            for (k = 0; k < RC; k++) {
                arg[k + 1] = m1[i][k];
            }
            for (k = 0; k < RC; k++) {
                arg[k + RC + 1] = m2[k][j];
            }

            err = pthread_create(&(tid[count++]), NULL, &mul, (void*)arg);
            if (err != 0)
                printf("\n can't create thread : [%s]", strerror(err));
        }
    }

    printf("\nHasil Perkalian :\n");
    for (i = 0; i < R * C; i++) {
        void* k;
        pthread_join(tid[i], &k);
        int* p = (int*)k;

        printf("%d ", *p);
        if ((i + 1) % C == 0) {
            printf("\n");
        }
        m_res[i] = *p;
    }

    for (i = 0; i < R * C; i++) {
        res[i] = m_res[i];
    }

    shmdt(res);
    exit(0);
    return 0;
}
