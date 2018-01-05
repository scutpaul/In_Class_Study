/**********************
filename:ProducterAndConsumer_update.c
creator:scut_paul
create_time:2017.11.03

type:os_exp_02
work:this program create 5 processes.
2 processes are Producters.
one keep producting capital letters in cache.
another keep producting capital letters in cache.
3 processes are consumers.
each of them keep getting and printing letter.
**********************/
/*有名信号量用于同步线程与相关、不相关进程、函数sem_open，存储信号量在文件下；
无名信号量用于同步线程与相关进程，函数sem_init，存储信号量在内存中*/

#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include <unistd.h>        //sleep
#include <sys/shm.h>       //shmget\shmat
#include <sys/ipc.h>       //shmget
#include <sys/types.h>     //shmat
#include <semaphore.h>     //sem
#define TRUE 1
#define N 100 /*缓冲区的槽数目*/
#define SHM_SIZE (1024*1024)
#define SHM_MODE 0666 //可以读取
int N_PRODUCER = 2;
int N_CONSUMER = 3;
int ShMId = -1;
/*共享存储*/
struct ShM{
    sem_t mutex;//互斥访问临界区
    sem_t empty;//空槽
    sem_t full;//满槽

    int insert_index;
    int read_index;
    char cache[N];
}*pSM;

char produce_item(int producer_id){
    if(producer_id == 0){
        return 'A';
    }else if(producer_id == 1){
        return 'a';
    }else{
        return 'n';
    }
}
void insert_item(char item){
    pSM->cache[pSM->insert_index]=item;
    pSM->insert_index=((pSM->insert_index)++)%N;
}

int remove_item(int consumer_id){  //修改不同消费者消费特定物品
    char item = pSM->cache[pSM->read_index];
    if((consumer_id<2&&item=='A')||(consumer_id>0&&item=='a')){//0为大写；1为通吃；2为小写
        printf("消费者%d输出：%c\n", consumer_id,item);
        pSM->read_index=((pSM->read_index)++)%N;
        return 1;
    }else{
        return 0;
    }
}
void init(){

    if ((ShMId = shmget((key_t)1234,SHM_SIZE,SHM_MODE|IPC_CREAT))<0){
        printf("03");
        perror("create shared memory failed");
        exit(1);
    }
    printf("03");
    pSM = (struct ShM *)shmat(ShMId,0,0);
    if(pSM == (void*)-1)
    {
      perror("shmat failed");
      exit(1);
    }
    printf("03");
    pSM->insert_index = 0;
    pSM->read_index = 0;
    sem_init(&(pSM->mutex),1,1);
    printf("02");
    sem_init(&(pSM->empty),1,N);
    sem_init(&(pSM->full),1,0);
}
void end(){
    sem_destroy(&(pSM->empty));
    sem_destroy(&(pSM->full));
    sem_destroy(&(pSM->mutex));
    if(shmdt(pSM) == -1)
    {
        perror("shmdt failed");
        exit(1);
    }
    if(shmctl(ShMId, IPC_RMID, 0) == -1)
    {
        perror("shmctl failed");
        exit(1);
    }
}
int main(){
    init();
    pid_t child = -1;
    for(int i = 0;i<N_CONSUMER;i++){
        child = fork();
        if(child<0){
            perror("the fork failed");
            exit(1);

        }
        else if(child == 0){
            printf("consumer");
            while(1){
                sem_wait(&(pSM->full));
                sem_wait(&(pSM->mutex));
                if(remove_item(i)==1){//消费成功则正常
                    sem_post(&(pSM->mutex));
                    sem_post(&(pSM->empty));
                }else{//消费失败回退消费前
                    sem_post(&(pSM->mutex));
                    sem_post(&(pSM->full));
                }
                sleep(1);
            }
            break;
        }
    }
    for(int i = 0;i<N_PRODUCER;i++){
        child = fork();
        if(child<0){
            perror("the fork failed");
            exit(1);
        }
        else if(child == 0){
            char item;
            while(TRUE){
                item = produce_item(i);
                sem_wait(&(pSM->empty));
                sem_wait(&(pSM->mutex));
                insert_item(item);
                sem_post(&(pSM->mutex));
                sem_post(&(pSM->full));
                sleep(1);
            }
            break;
        }
    }
    return 0;
}
