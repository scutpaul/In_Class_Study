//
// Created by paul on 17-12-30.
//

#ifndef SOCKET_IPV6_SERVER_CONF_H
#define SOCKET_IPV6_SERVER_CONF_H

#endif //SOCKET_IPV6_SERVER_CONF_H


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
#define MAX_LINE 280
#define PORT  8888
#define LISTENEQ  6000
/*定义服务器--客户端 消息传送类型*/
enum MessageType{
    LOGIN=1,		/*登陆请求*/
    EXIT,				/*退出请求*/
    VIEW_USER_LIST,		/*查看在线列表*/
    CHAT,
    RESULT			/*结果消息类型*/
};

/*定义服务器 -- 客户端 消息传送结构体*/
typedef struct _Message{
    char content[211];		/*针对聊天类型的消息，填充该字段*/
    int msgType;	/*消息类型 即为MessageType中的值*/
    int msgRet;		/*针对操作结果类型的消息，填充该字段*/
    struct sockaddr_in6 sendAddr; /*发送者IP*/
    char sendName[20]; /*发送者名称*/
    char msgTime[9];  /*消息发送时间*/
}Message;

enum StateRet{
    SUCCESS, //成功
    DUPLICATEID, //重复的用户名
};
//用户信息结构体
typedef struct _User{
    char userName[20]; 		//用户名
    struct sockaddr_in6 userAddr;	//用户IP地址，选择IPV4
    int sockfd;			//当前用户套接字描述符
}User;

/*定义用户链表结构体*/
typedef struct _UserList{
    User user;
    struct _UserList *next;
}UserList;


/*定义在线用户链表*/
//UserList *userList;
