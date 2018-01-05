/**********************
 * creator:scutpaul
 * file:smyfind.c
 * function:myfind Path -option [-print] [-exec .]
 * max file max_file 10240
 * max dir word max_n 1024
 * ***********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define one_day_sec 86400
#define max_file 10240
#define max_n 1024

int name_choice = 0;
int prune_choice = 0;
int mtime_choice = 0;
int ctime_choice = 0;
int print_choice = 0;
int exec_choice = 0;

char *name_context, *prune_contex,*exec_context[max_n];
int c_time = 0;
int m_time = 0;

char matchfile[max_file][max_n];
int matchfile_index = -1;
int exec_index = 0;

void printf_help(){
    printf("help:\n");
    printf("myfind PATH -option 参数 [-print] [-exec 命令]\n");
    printf("PATH：查找的起始目录。\n");
    printf("-option 参数：用来控制搜索方式\n");
    printf("\t-name \"文件\"：指定要查找的文件名，可以有通配符?*等。\n");
    printf("\t-prune 目录：指出搜索时不搜索该目录。\n");
    printf("\t-mtime +n或-n：按时间搜索，+n：表示n天之前修改过的文件；-n：表示今天到n天前之间修改过的文件。\n");
    printf("\t-ctime +n或-n：也是按时间查找，+n：表示n天之前创建的文件；-n：表示今天到n天前之间创建的文件。\n");
    printf("\t-print：将搜索结果输出到标准输出。\n");
    printf("-exec 程序名\n");
}
/***************************
 * input Dir "/home/paul"
 * op matchfile
 * function:"find the child dir file"
 * */
void search_dir(char *dir){
    DIR* dir_path;
    struct dirent *filename;
    dir_path = opendir(dir);
    if(!dir_path){
        printf("%s--:",dir);
        perror("error open dir\n");
        return;
    }
    char str[max_n];
    char chile_path[max_n];
    while(filename = readdir(dir_path)){
        if(strcmp(filename->d_name,".")==0||strcmp(filename->d_name,"..")==0)
            continue;
        strcpy(str,dir);
        sprintf(chile_path,"%s/%s",str,filename->d_name);
        if(filename->d_type==DT_DIR){
            if(prune_choice&&strcmp(chile_path,prune_contex)==0)
                continue;
            search_dir(chile_path);
        }
        else{
            int final_result = 0;
            struct stat test;
            //fnnmatch could match with *?
            if(name_choice){
                int fm=fnmatch(name_context,filename->d_name,0);
                if (fm == 0)final_result=1;
            }
            int status = stat(str,&test);
            time_t current;
            time(&current);
            int m_day=(int)(current-test.st_mtim.tv_sec)/one_day_sec;
            int c_day=(int)(current-test.st_ctim.tv_sec)/one_day_sec;
            if(mtime_choice){
                if(m_time>0){
                    if(m_day>=m_time&&final_result)
                        final_result = 1;
                    else{
                        final_result = 0;
                    }
                }else{
                    if(m_day+m_time<0&&final_result)
                        final_result = 1;
                    else{
                        final_result = 0;
                    }
                }
            }
            if(ctime_choice){
                if(c_time>0){
                    if(c_day>=c_time&&final_result)
                        final_result = 1;
                    else
                        final_result = 0;
                }else{
                    if(c_day+c_time<0&&final_result)
                        final_result = 1;
                    else
                        final_result = 0;
                }
            }
            if(final_result){
                strcpy(matchfile[++matchfile_index],chile_path);
            }

        }
    }
    closedir(dir_path);
}

int main(int argc,char *argv[]) {
    if(argc<=2){
        printf_help();
        exit(1);
    }
    /*set choice*/
    int cnt;
    for(cnt = 2;cnt<argc;cnt++){
        if(strcmp(argv[cnt],"-name")==0){
            name_choice = 1;
            name_context = argv[++cnt];
        }else if(strcmp(argv[cnt],"-prune")==0){
            prune_choice = 1;
            prune_contex = argv[++cnt];
        }
        else if(strcmp(argv[cnt],"-mtime")==0){
            mtime_choice = 1;
            if(atoi(argv[++cnt])==0){
                printf("m time error\n");
                exit(1);
            }
            m_time = atoi(argv[cnt]);

        } else if(strcmp(argv[cnt],"-ctime")==0){
            ctime_choice = 1;
            if(atoi(argv[++cnt])==0){
                printf("c time error\n");
                exit(1);
            }
            c_time = atoi(argv[cnt]);
        }else if (strcmp(argv[cnt],"-print")==0){
            print_choice = 1;
        }else if(strcmp(argv[cnt],"-exec")==0){
            exec_choice = 1;
            //while(cnt<argc && strcmp(argv[cnt+1],";")){
               // printf("%s\n",argv[cnt+1]);
            exec_context[exec_index++] = argv[++cnt];
            //}
        }else{
            printf("commond error");
            printf_help();
            exit(0);
        }
    }
    /*find dir*/
    search_dir(argv[1]);
    if(print_choice){
        //printf("%d",matchfile_index);
        //fflush(stdout);
    for (int i = 0;i<=matchfile_index;i++){
        printf("%s\n",matchfile[i]);
    }
    }

    if(exec_choice){
        pid_t pid;
        pid = fork();
        if (pid==0){
            for(int i = 0;i <=matchfile_index;i++){
                exec_context[exec_index++] = matchfile[i];
            }
            execvp(exec_context[0],exec_context);
        } else if(pid>0){
            wait(NULL);
        }
    }


    return 0;
}