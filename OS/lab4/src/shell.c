/********************
Filename:shell.c
Author:scut_paul
Created:2017.11.24

type:os_ex04
work: make a sell
*********************/
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h> //getpwuid()
#include <unistd.h> //getuid chdir getcwd gethostname
#include "memory.h"
#include "stdlib.h"
#include "wait.h"


int main(int agrc, char *argv[]){
    uid_t my_uid;
    struct passwd *my_info;
    while(1) {
        char hostname[50];
        char buf_cwd[1024];
        char print_cwd_str[1024]; //temp 
        char print_cwd[1024];   //print_now_cwd:usr_dir ~
        char *buf;             //common line input
        char *buf_token[1024];  //token[]
        size_t buf_len;
        size_t token_num;
        int backstage;
        buf = NULL;
        token_num = 0;
        backstage = 0;
        buf_len = 0;
        my_info = getpwuid(getuid());
        gethostname(hostname, sizeof(hostname));
        getcwd(buf_cwd, sizeof(buf_cwd));
        if(strlen(buf_cwd)>=strlen(my_info->pw_dir)) {
            for (int j = 0; j < strlen(buf_cwd); j++) {
                if (j < strlen(my_info->pw_dir))
                    print_cwd_str[j]=buf_cwd[j];
            }
            print_cwd_str[strlen(my_info->pw_dir)]='\0';
            if(strcmp(print_cwd_str,my_info->pw_dir)==0){
                print_cwd[0]='~';
                int i = 0;
                int j ;
                for(j= strlen(my_info->pw_dir);j<strlen(buf_cwd);j++){
                    print_cwd[++i]=buf_cwd[j];
                }
                print_cwd[++i]='\0';
            } else{
                strcpy(print_cwd,buf_cwd);
            }
        }
        else{
            strcpy(print_cwd,buf_cwd);
        }
        printf("[*]%s@%s:%s", my_info->pw_name, hostname, print_cwd);
        if (geteuid() == 0)printf("#");
        else printf("$");
        getline(&buf, &token_num, stdin);
        buf_len = strlen(buf);
        if (buf_len == 1) continue;
        buf[buf_len - 1] = '\0';
        buf_token[0] = strtok(buf," ");
        token_num = 0;
        do {
            token_num++;
            buf_token[token_num]=strtok(NULL," ");
        }while(buf_token[token_num]!=NULL);
        if(strcmp(buf_token[token_num-1],"&")==0) {
            backstage = 1;
            buf_token[token_num-1]=NULL;
        }
        if(strcmp(buf_token[0],"exit")==0)
            if(buf_token[1]!=NULL)
                printf("commond:exit no allow follow\n");
            else
                exit(0);
        else if(strcmp(buf_token[0],"cd")==0)
            if(buf_token[1]==NULL)
                continue;
            else if(buf_token[2]==NULL) {
                if(strcmp(buf_token[1],"~")==0){
                    chdir(my_info->pw_dir);
                }else if (chdir(buf_token[1]) != 0) {
                    perror(NULL);
                }
            }
            else{

                printf("commond: cd <Path>\n");
            }
        else{
            pid_t pid;
            pid = fork();
            if(pid < 0){
                printf("pthread don't create\n");
                exit(1);
            }else if(pid == 0){
                if(execvp(buf_token[0],buf_token)<0){
                    printf("commond error\n");
                    exit(2);
                }
            }else {
                if(!backstage){
                    wait(NULL);
                }
            }
        }
        //printf("%d:%d", token_num, strlen(buf));
        //break;
    }
    return 0;
    //chdir();
}