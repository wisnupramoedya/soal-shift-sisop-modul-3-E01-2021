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
#define C 6

void* factorial(void* argvar){
	int *arg = (int*)argvar;
     	int i,j,selisih,fac=1;

	for(i=0; i<=3; i++){
		for(j=i*12; j<i+6; j++){
			if(arg[j]==0 || arg[j+6]==0)
				fac=0;
			else if(arg[j] >= arg[j+6]){
				selisih=arg[j]-arg[j+6];
				for(j=arg[j]; j>selisih; j--){
					fac*=arg[j];
				}
			}
			else if(arg[j+6] > arg[j]){
				for(j=arg[j]; j>=1; j--){
					fac*=arg[j];
				}
			}
		}
	}
	
	int *p=(int*)malloc(sizeof(int));
	*p=fac;
	pthread_exit(p);
}

int main(){
	
	key_t key = 1945;
	
	int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666 );
	int *res = (int *)shmat(shmid, NULL, 0);
	
	int m1[R][C],m2[R][C];
	
        int i,j,k=0,l=0;
	for(i=0; i < R; i++){
		for(j=0; j < C; j++){
			m1[i][j] = res[k++];
		}
	}

	printf("\nMatriks A :\n");
	for(i=0; i < R; i++){
		for(j=0; j < C; j++)
			printf("%d ",m1[i][j] );
		printf("\n");
	}
	
	printf("\nMatriks B :\n");
	for(i=0; i < R; i++){
		for(j=0; j < C; j++){
			scanf("%d",&m2[i][j]);
		}
	}

	int err;
	pthread_t *tid=(pthread_t*)malloc((24)*sizeof(pthread_t));

	int count=0;
	int* arg=NULL;
	k=0;
	l=6;
	
	arg = (int*)malloc((48)*sizeof(int));
	for(i=0; i < R; i++){
        	for(j=0; j < C; j++){
			//arg[0]=6;
			arg[k++] = m1[i][j];
			arg[l++] = m2[i][j];
			err = pthread_create(&(tid[count++]),NULL,factorial,(void*) arg);
			if(err!=0)
				printf("\n can't create thread : [%s]",strerror(err));
		}
		k+=6;
		l+=6;
    	}

	//cek merge array
	printf("\nMerge matriks :\n");
	for(i=0; i < 48; i++){
		if(i%12==0 && i!=0)
			printf("\n");
		printf("%d ",arg[i]);
	}

	printf("\n\nHasil Faktorial :\n");
	for(i=0; i < 24; i++){
		void*k;
		pthread_join(tid[i],&k); 
		int* p = (int* )k;
		if(i%6==0 && i!=0)
			printf("\n");
		printf("%d ",*p);
	}
	printf("\n");
	return 0;
}
