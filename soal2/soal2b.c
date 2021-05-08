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
#define C 6

int matrix[R][C], matrix_shared[R][C], matrix_new[R][C];

pthread_t tid[R * C];

typedef struct thread_args {
    int A, B, i, j;
} args;

int factorial(int a, int b) {
    int result = 1, i;
    // printf("%d:%d\n", a, b);
    if (a == 0 || b == 0)
        result = 0;
    else if (a >= b)
        for (i = a - b + 1; i <= a; i++) result *= i;
    else if (a < b)
        for (i = 1; i <= a; i++) result *= i;

    return result;
}

void *calculator(void *arg) {
    pthread_t id = pthread_self();
    int i;

    for (i = 0; i < R * C; i++) {
        if (pthread_equal(id, tid[i])) {
            args *arg_matrix = (args *)arg;

            matrix[arg_matrix->i][arg_matrix->j] =
                factorial(arg_matrix->A, arg_matrix->B);

            // printf("A:B,a:b -> res => %d:%d, %d:%d -> %d\n", arg_matrix->A,
            //        arg_matrix->B, arg_matrix->i, arg_matrix->j,
            //        matrix[arg_matrix->i][arg_matrix->j]);
        }
    }
}

int main(int argc, char const *argv[]) {
    int i, j, k, err;
    args arg[R * C];

    key_t key = 1945;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    int *result = (int *)shmat(shmid, NULL, 0);

    printf("Matrix A:\n");
    for (i = 0; i < R; i++) {
        for (j = 0; j < C; j++) {
            matrix_shared[i][j] = result[k++];
            printf("%d ", matrix_shared[i][j]);
        }
        printf("\n");
    }

    printf("\nMatrix B:\n");
    for (i = 0; i < R; i++) {
        for (j = 0; j < C; j++) {
            // printf("%d:%d ", i, j);
            scanf("%d", &matrix_new[i][j]);
            // printf("%d ", matrix_new[i][j]);
        }
        // printf("\n");
    }

    for (i = 0; i < R; i++) {
        for (j = 0; j < C; j++) {
            arg[i * C + j].A = matrix_shared[i][j];
            arg[i * C + j].B = matrix_new[i][j];
            arg[i * C + j].i = i;
            arg[i * C + j].j = j;
            // printf("%d => %d %d %d %d\n", i * C + j, arg[i * C + j].A,
            //        arg[i * C + j].B, arg[i * C + j].i, arg[i * C + j].j);

            err = pthread_create(&(tid[i * C + j]), NULL, calculator,
                                 &arg[i * C + j]);
            if (err) printf("Error");
        }
    }

    for (i = 0; i < R * C; i++) pthread_join(tid[i], NULL);

    printf("\nResult:\n");
    for (i = 0; i < R; i++) {
        for (j = 0; j < C; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    shmdt((void *)result);
    return 0;
}
