//
// Created by paul on 17-12-25.
//
#ifndef SOCKET_CONF_H
#define SOCKET_CONF_H

#endif //SOCKET_CONF_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h> /*使用memcpy所需的头文件*/
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>

#define nullptr NULL
#define MAX_LINE 268
#define PORT  8888
#define LISTENEQ  6000
enum MessageType{
    LOGIN=1,EXIT,VIEW_USER_LIST,CHAT,RESULT
};
typedef struct _Message{
    char content[210];
    int msgType;
    int msgRet;
    struct sockaddr_in sendAddr; 
    char sendName[20]; 
    char msgTime[9]; 
}Message;
enum StateRet{
    SUCCESS, 
    DUPLICATEID, //重复的用户名
    ALL_NOT_ONLINE
};
typedef struct _User{
    char userName[20]; 		
    struct sockaddr_in userAddr;	
    int sockfd;		
}User;
typedef struct _ListNode{
    User user;
    struct _ListNode *next;
}ListNode;
