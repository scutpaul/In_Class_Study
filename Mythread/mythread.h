#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <malloc.h>

#define mythread_t unsigned long int
#define STACKCAPACITY 40000 
#define NEWTHREAD 0
#define RUNNING 1
#define COMPLETE 2
#define EXITED 3
#define CANCEL 4
#define BLOCKED 5

struct mythread {
	int state;
	mythread_t tID;
	ucontext_t tContext;
	void *retval;
	struct mythread *next;
} *first, *main_thread, *current, *end;

typedef struct mythread_mutex_t {
	long lock;
	mythread_t owner;
} mythread_mutex_t;

void mythread_init(long period);
int  mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);
int  mythread_join(mythread_t thread, void **status);
void mythread_exit(void *retval);
void mythread_yield(void);
int  mythread_equal(mythread_t t1, mythread_t t2);
int  mythread_cancel(mythread_t thread);
mythread_t mythread_self(void);
int  mythread_mutex_init(mythread_mutex_t *mutex);
int  mythread_mutex_lock(mythread_mutex_t *mutex);
int  mythread_mutex_unlock(mythread_mutex_t *mutex);
