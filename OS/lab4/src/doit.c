/********************
Filename:doit.c
Author:scut_paul
Created:2017.11.24

type:os_ex04
work:
*********************/
#include<stdio.h>
#include<stdlib.h>
#include<sys/resource.h>
#include<sys/time.h>
#include<unistd.h>     //execve
#include <sys/types.h> //wait
#include <sys/wait.h> //wait
#define N 100
int main(int argc,char**argv){
    char* input[N];
    struct rusage resource_usage;
    struct timeval start_time, end_time;
    pid_t pid = -1;
    for(int i = 0; i < argc; i++){
        input[i] = argv[i+1];
    }
    input[argc-1]=(char*)0;
    pid = fork();
    gettimeofday(&start_time,NULL);
    if(pid==0){
        char*envp[]={0,NULL};
        if(execve(input[0],input,envp)!=0){
            printf("error run child process");
            exit(0);
        }
    }else if(pid>0){
        int status;
        wait(&status);
        gettimeofday(&end_time,NULL);
        if(getrusage(RUSAGE_CHILDREN,&resource_usage)!=0){
            printf("get usage error");
            exit(0);
        }
	//printf("test:%ld\n",resource_usage.ru_utime.tv_usec);
        double usr_cpu = (double)resource_usage.ru_utime.tv_sec*1000+(double)(resource_usage.ru_utime.tv_usec-0)/1000;
        double sys_cpu = (double)resource_usage.ru_stime.tv_sec*1000+(double)(resource_usage.ru_stime.tv_usec-0)/1000;
        double run_time = (double)(end_time.tv_sec-start_time.tv_sec)*1000+(double)(end_time.tv_usec-start_time.tv_usec)/1000;
	
        long be_involuntary_switched = resource_usage.ru_nivcsw;
        long voluntary_switche = resource_usage.ru_nvcsw;
        long page_faults = resource_usage.ru_minflt+resource_usage.ru_majflt;
        printf("命令执行占用的CPU时间（用户态）：%fms\n",usr_cpu );
        printf("命令执行占用的CPU时间（系统态）:%fms\n",sys_cpu );
        printf("命令执行时间：%fms\n",run_time );
        printf("进程被抢占的次数：%ld\n",be_involuntary_switched );
        printf("进程主动放弃CPU的次数：%ld\n", voluntary_switche);
        printf("进程执行过程中发生缺页的次数：%ld\n",page_faults );
    }
    else{
        printf("fork error\n");
        exit(0);
    }
}
