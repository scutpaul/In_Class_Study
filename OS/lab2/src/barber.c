/**********************
filename:barber.c
creator:scut_paul
create_time:2017.11.03

type:os_exp_02
work:this program create barber model.
there are n customers_chairs and 1 barber_chair in this barber shop.
when no customer in the shop, the barber go to sleep.
when customer come, the barber is woken up.
**********************/
#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include <unistd.h>        //sleep
#include <sys/shm.h>       //shmget\shmat
#include <sys/ipc.h>       //shmget
#include <sys/types.h>     //shmat
#include <semaphore.h>     //sem
#include <pthread.h>       //thread

#define N 5  //五张椅子
int free_chairs = N;

sem_t mutex;
sem_t wait;//实时查看人数
sem_t cut;

void *customer(void *ptr){
	int *id_index = (int*)ptr;
	int id = *id_index;
	printf("顾客%d：=========进门\n",id);
	sem_wait(&mutex);
	if(free_chairs<=0){
		sem_post(&mutex);
		printf("顾客%d：*********人太多了，没剪\n",id);
		return 0;
	}
	free_chairs--;
	printf("旁白：还可以坐%d人的\n",free_chairs );
	sem_post(&mutex);
	sem_post(&cut);
	sem_wait(&wait);
	sleep(2);
	printf("顾客%d：++++++++剪好了离开\n",id);
	return 0;
}
void *barber(void *ptr){
	//int p=-1;
    while(1){
		//sem_getvalue(&cut,&p);
		//printf("%d\n",p);
        sem_wait(&cut);
		sem_post(&wait);
		printf("理发师：--------剪发了\n");
		sem_wait(&mutex);
		free_chairs++;
		sem_post(&mutex);
		sleep(2);
    }
}
void init(){
    sem_init(&mutex,0,1);
	sem_init(&cut,0,0);
	sem_init(&wait,0,0);
}
int main(){
	init();
	pthread_t barber_id;
	pthread_t cus_thread[N*50];
	int cid[N*50];
	int bar = pthread_create(&barber_id,NULL,barber,NULL);
	if(bar){
		printf("create failed\n");
		return 1;
	}
	for(int i = 0;i < N*50;i++){
		cid[i]=i;
		int cus = pthread_create(&cus_thread[i],NULL,customer,&cid[i]);
		if(cus){
			printf("create customer failed");
			return 1;
		}
		sleep(1);
	}
	pthread_join(bar,NULL);
	for(int i = 0;i < N*50;i++){
		pthread_join(cus_thread[i],NULL);
	}

}
