/**********************
filename:ReaderAndWriter.c
creator:scut_paul
create_time:2017.11.03

type:os_exp_02
work:this program create reader-writer model.
the writers process take precedence in this model.
when the writers come, the reader must to be writting.
**********************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/ipc.h>       //shmget
#include <sys/types.h>     //shmat
#include <semaphore.h>
#define Random(x) (rand() % x)

#define SHM_SIZE (1024*1024)
#define SHM_MODE 0666 //可以读取

int ShMId = -1;
struct ShM{
	int writers;
	int readers;
	sem_t mutex;
	sem_t reader;
	sem_t writer;
	sem_t mutex_r;
	sem_t mutex_ts;
}*pSM;

void init(){
	if ((ShMId = shmget((key_t)1234,SHM_SIZE,SHM_MODE|IPC_CREAT))<0){
        perror("create shared memory failed");
        exit(1);
    }
    pSM = (struct ShM *)shmat(ShMId,0,0);
    if(pSM == (void*)-1)
    {
      perror("shmat failed");
      exit(1);
    }
	pSM->writers = 0;
	pSM->readers = 0;
	sem_init(&pSM->mutex,1,1);
	sem_init(&pSM->writer,1,1);
	sem_init(&pSM->reader,1,1);
	sem_init(&pSM->mutex_r,1,1);
	sem_init(&pSM->mutex_ts,1,1);
}
void reader_do(int reader_id){
	printf("reader%d:--------in\n",reader_id);
	sem_wait(&pSM->mutex_r);
	sem_wait(&pSM->reader);
	sem_wait(&pSM->mutex);
	pSM->readers++;
	if(pSM->readers==1)
		sem_wait(&pSM->writer);
	sem_post(&pSM->mutex);
	sem_post(&pSM->reader);
	sem_post(&pSM->mutex_r);

	printf("reader%d:reading....\n",reader_id);
	sleep(3);
	printf("reader%d:read-----done\n",reader_id);

	sem_wait(&pSM->mutex);
	pSM->readers--;
	if(pSM->readers==0)
		sem_post(&pSM->writer);
	sem_post(&pSM->mutex);
	exit(0);
}
void writer_do(int writer_id){
	int temp = writer_id;
	printf("writer%d---------in\n",temp);
	sem_wait(&pSM->mutex_ts);
	pSM->writers++;
	if (pSM->writers == 1)
		sem_wait(&pSM->reader);
	sem_post(&pSM->mutex_ts);

	sem_wait(&pSM->writer);
	printf("writer%d:writting...\n",temp);
	sleep(3);
	printf("writer%d:writting-----done\n",temp);
	sem_post(&pSM->writer);

	sem_wait(&pSM->mutex_ts);
	pSM->writers--;
	if (pSM->writers == 0)
		sem_post(&pSM->reader);
	sem_post(&pSM->mutex_ts);
	exit(0);
}
int main(){
	init();
	int dis = 3;
	pid_t child = -1;
	int reader_id = 0;
	int writer_id = 0;
	int rand_num;
	for(int i = 0;i<200;i++){
		rand_num = Random(dis);
		if(rand_num == 0){
			writer_id++;
		}else{
			reader_id++;
		}
		child = fork();
		if(child < 0){
			printf("fork failed\n");
			return 1;
		}
		else if(child == 0){
			if(rand_num == 0){
				writer_do(writer_id);
			}else{
				reader_do(reader_id);
			}
		}
		sleep(1);
	}
	return 0;
}
