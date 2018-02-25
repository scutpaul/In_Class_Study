#include <stdio.h>
#include <stdint.h>
#include "mythread.h"

mythread_t philosopher[5];
mythread_mutex_t chopstick[5];

void thinking (int id) {
	long int i = 0, think_time = rand()%10000000;
	printf("Philosopher #%d is thinking...\n", id);
	for (i=0; i<think_time; i++);
}

void get_chopsticks (int id) {
	if (id%2 == 0) {
	mythread_mutex_lock(&chopstick[id]);
	printf("Philosopher #%d got left chopstick.\n", id);
	mythread_mutex_lock(&chopstick[(id+1)%5]);
	printf("Philosopher #%d got right chopstick.\n", id);
	} else {
	mythread_mutex_lock(&chopstick[(id+1)%5]);
	printf("Philosopher #%d got right chopstick.\n", id);
	mythread_mutex_lock(&chopstick[id]);
	printf("Philosopher #%d got left chopstick.\n", id);
	}
}

void put_chopsticks (int id) {
	if (id%2 == 0) {
	mythread_mutex_unlock(&chopstick[(id+1)%5]);
	printf("Philosopher #%d put right chopstick.\n", id);
	mythread_mutex_unlock(&chopstick[id]);
        printf("Philosopher #%d put left chopstick.\n", id);
	} else {
	mythread_mutex_unlock(&chopstick[id]);
        printf("Philosopher #%d put left chopstick.\n", id);
	mythread_mutex_unlock(&chopstick[(id+1)%5]);
	printf("Philosopher #%d put right chopstick.\n", id);
	}
}

void eatting (int id) {
	long int i = 0, eat_time = rand()%1000000000;
	get_chopsticks(id);
	printf("Philosopher #%d is eating...\n", id);
	for (i=0; i<eat_time; i++);
	put_chopsticks(id);
}

void *dining (int n) {
	while(1) {
		thinking(n);
		printf("Philosopher #%d is hungry...\n", n);
		eatting(n);
		printf("Philosopher #%d finished eating\n", n);
	}
}

void main () {
	int i;
	mythread_init(100000);
	for (i=0; i<5; i++) {
		mythread_mutex_init(&chopstick[i]);
		mythread_create(&philosopher[i], (void *)dining, (int *)(i));
	}
	mythread_exit(NULL);
}
