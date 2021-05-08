#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>

#define R 4
#define RC 3
#define C 6

void* mul(void* struk){
	int *arg = (int*)struk;
     	int k=0, i=0;

	int x=arg[0];
	for(i=1; i<=x; i++)
		k+=arg[i]*arg[i+x];
	
	int *p=(int*)malloc(sizeof(int));
	*p=k;
	pthread_exit(p);
}

int m1[R][RC] = { {1,2,3}, {2,3,1}, {3,1,2}, {3,2,1} },
    m2[RC][C] = { {1,2,1,2,1,2}, {2,1,2,1,2,1}, {1,1,2,2,1,1} },
    m_res[R*C];

int main(){
	
	key_t key = 2021;
	
	int shmid = shmget(key, sizeof(int)*R*C, IPC_CREAT | 0666 );
	int *res = (int *)shmat(shmid, NULL, 0);
	
        int i,j;
        printf("Matriks %dx%d :\n", R, RC);
	for(i=0; i < R; i++){
		for(j=0; j < RC; j++)
			printf("%d ",m1[i][j] );
		printf("\n");
	}

	printf("\nMatriks %dx%d :\n", RC, C);
	for(i=0; i < RC; i++){
		for(j=0; j < C; j++)
			printf("%d ",m2[i][j] );
		printf("\n");
	}

	int err,k;
	pthread_t *tid=(pthread_t*)malloc((24)*sizeof(pthread_t));

	int count=0;
	int* arg=NULL;

	for(i=0; i < R; i++){
        	for(j=0; j < C; j++){
			arg = (int*)malloc((24)*sizeof(int));
			arg[0]=RC;

			for	(k=0; k<RC; k++){
				arg[k+1]=m1[i][k];
			}
			for	(k=0; k<RC; k++){
				arg[k+RC+1]=m2[k][j];
			}

				
			err = pthread_create(&(tid[count++]),NULL,&mul,(void*) arg);
			if(err!=0)
				printf("\n can't create thread : [%s]",strerror(err));
		}
    	}

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


	for(i=0; i < R*C; i++){
            res[i] = m_res[i];
        
	}

	shmdt(res);
	exit(0);
	return 0;
}