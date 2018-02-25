
#include "mythread.h"
/*
void who(int id){
    switch (id){
        case 0:
            printf("im : %ld\n",mythread_self());
            mythread_exit("123");
            break;
        case 1:
            printf("im : %ld\n",mythread_self());
            for(;;);
            break;
        case 2:
            printf("im : %ld\n",mythread_self());
            if(mythread_cancel(2)==0)printf("cancel sucee\n");
            show_all_thread();
            break;
        case 3:
            //for (;;);

            break;
    }
}*/
void who1(int id){
    for(;;);
}
int main(){
    char * a = NULL;
    mythread_init(100000,5);
    //printf("id:%ld",mythread_self());
    mythread_t t[4];

    for(int i = 0;i<4;i++){
        mythread_create(&t[i],i+1,(void*)who1,(int*)i);
    }
    for(;;);
    //mythread_join(1,(void * )&a);
    //printf("join-end:%s",a);
    //show_all_thread();
    mythread_exit(NULL);
}

