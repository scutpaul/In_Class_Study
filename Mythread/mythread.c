/*
@author: scutpaul
@more:201530311042
*/
#include "mythread.h"

mythread_t newtid; //new_thread_id;

int debug = 0;

/*显示当前线程库所有线程状态*/
void show_all_thread(){
    if(debug) {
        printf("\n------------all-thread-begin------------\n");
        struct mythread *p = first;
        while (!mythread_equal(p->tID, end->tID)) {
            printf("thread:%ld   state:%d   \n", p->tID, p->state);
            p = p->next;
        }
        printf("thread:%ld   state:%d   \n", p->tID, p->state);
        printf("\n------------all-thread-end------------\n");
    }
}
/*时间片调度-无优先级-切换线程*/
void switch_thread(){
    struct mythread * pre = current;
    current = current->next;
    while(current->state!=RUNNING&&current->state!=NEWTHREAD){
        current = current->next;
    }
    if(current->state == NEWTHREAD)
        current->state = RUNNING;
    if(!mythread_equal(pre->tID,current->tID)){
        swapcontext(&pre->tContext,&current->tContext);
    }
}
/*时间片调度-信号处理函数*/
void sigroutine(int signo){
    switch (signo) {
        case SIGVTALRM:  //切换信号
            if (first == NULL){ //未初始化
                printf("Signo: 线程库未初始化\n");
                break;
            }
            switch_thread();
            break;
        default:
            printf("Signo: unknown signo\n");
            break;
    }
}

void mythread_init(long period){
    printf("------mythread-init------\n");

    signal(SIGVTALRM,sigroutine);  //定时信号处理函数绑定
    long Period;  //time_period
    struct itimerval value, ovalue;
    Period = period/1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = Period;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = Period;
    setitimer(ITIMER_VIRTUAL,&value,&ovalue);  //定时信号设置

    first = malloc(sizeof(* first)); //线程库环境初始化
    main_thread = first;
    current = first;
    end = first;
    newtid = 0;
    newtid++;

    getcontext(&main_thread->tContext); //主线程初始化
    main_thread->next = end;
    main_thread->tID = 0;
    main_thread->state = RUNNING;
    main_thread->tContext.uc_stack.ss_sp = malloc(STACKCAPACITY);
    main_thread->tContext.uc_stack.ss_size = STACKCAPACITY;
}

int  mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg){
    struct mythread *new = malloc(sizeof(*new));
    getcontext(&new->tContext);
    new->tID = newtid;
    newtid++;
    new->tContext.uc_stack.ss_sp = malloc(STACKCAPACITY);
    new->tContext.uc_stack.ss_size = STACKCAPACITY;
    new->state = NEWTHREAD;
    new->next = first;
    end->next = new;
    end = new;
    makecontext(&new->tContext,start_routine,1,arg);
    //mythread_yield();
    show_all_thread();
}

int  mythread_join(mythread_t thread, void **status){
    struct mythread *p = current->next;
    struct mythread *p_parent = current;
    int find_join = 0;
    while(!mythread_equal(p->tID,current->tID)){
        if(mythread_equal(p->tID,thread)) {
            find_join = 1;
            break;
        }
        p_parent = p_parent->next;
        p = p->next;
    }
    if (find_join){
        while(1) {
            if (p->state == EXITED || p->state == CANCEL || p->state == COMPLETE) {
                //*status = p->retval;
                void *status_p = p->retval;
                status = &status_p;
                p_parent->next = p->next;
                free(p);
                p = NULL;
                return 0;
            } else {
                mythread_yield();
            }
        }
    }else{
        return -1;
    }
}

void mythread_exit(void *retval){
    current->retval = retval;
    current->state = EXITED;
    mythread_yield();
}

void mythread_yield(void){
    if(current->state==BLOCKED){
        show_all_thread();
        for(;;){
            if(current->state==RUNNING) {
                show_all_thread();
                break;
            }
        }
    }
    if(current->state==EXITED){
        switch_thread();
    }
    if(current->state==RUNNING){
        switch_thread();
    }
}

int  mythread_equal(mythread_t t1, mythread_t t2){
    return t1 == t2;
}

int mythread_cancel(mythread_t thread){
    int find_cancel = 0;
    struct mythread * p = current->next;
    while(!mythread_equal(p->tID,current->tID)){
        if (mythread_equal(p->tID,thread)){
            find_cancel = 1;
            break;
        }
        p = p->next;
    }
    if (find_cancel) {
        p->state = CANCEL;
        return 0;
    }else
        return -1;
}

mythread_t mythread_self(void){
    return current->tID;
}

int  mythread_mutex_init(mythread_mutex_t *mutex){
    mutex->lock = 1;
}

int  mythread_mutex_lock(mythread_mutex_t *mutex){
    mutex->lock--;
    if(mutex->lock<0){
        mutex->owner=current->tID;
        current->state = BLOCKED;
        mythread_yield();
    }
}

int  mythread_mutex_unlock(mythread_mutex_t *mutex){
    mutex->lock++;
    //无须释放被锁的拥有者
    if(mutex->lock>0)
        return 1;
    int find_block = 0;
    struct mythread * p = first;
    while(!mythread_equal(p->tID,end->tID)){
        if(mythread_equal(mutex->owner,p->tID)){
            p->state = RUNNING;
            find_block = 1;
            break;
        }
        p = p->next;
    }
    if(mythread_equal(mutex->owner,p->tID)){
        p->state = RUNNING;
        find_block = 1;
    }
    return find_block;
}
