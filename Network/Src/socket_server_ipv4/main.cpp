/*******************************************************************************
* creator:scutpaul
* socket_server
********************************************************************************/

#include "conf.h"

/*定义全局变量 -- 在线用户链表*/
UserList *userList = nullptr;
/*用户的线程*/
void *ClientThread(void *argc);
UserList* insertUser(UserList *list , User *user);
int isUser(UserList *list , User *user);
void deleteUser(UserList *list , User *user);
char *Return_state(int state);
int loginIn(Message *msg , int sockfd);
int Chat(Message *msg, int sockfd);
int viewUserList(Message *msg , int sockfd);

int client_sockfd[FD_SETSIZE];
int idindex;

int main(void)
{
    int i , maxi , maxfd , listenfd , connfd , sockfd;
    socklen_t clilen;
    pthread_t pid;
    int opt = 1;
    struct sockaddr_in servaddr , cliaddr;
    fd_set rset , allset;
    int nready ;
    Message message;
    //printf("hi:%d", sizeof(message))
    char buf[MAX_LINE];
    //printf("hih:%d", sizeof(buf));
    //fflush(stdout);
    /*UserInfo*/
    User user;
    if((listenfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
        perror("socket error.\n");
        exit(1);
    }//if
    bzero(&servaddr , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    setsockopt(listenfd , SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt));
    if(bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
    {
        perror("bind error.\n");
        exit(1);
    }
    if(listen(listenfd , LISTENEQ) < 0)
    {
        perror("listen error.\n");
        exit(1);
    }
    /*首先初始化客户端描述符集*/
    maxfd = listenfd;
    maxi = -1;
    for(i=0; i<FD_SETSIZE; ++i)
    {
        client_sockfd[i] = -1;
    }
    /*清空allset描述符集*/
    FD_ZERO(&allset);
    /*将监听描述符加到allset中*/
    FD_SET(listenfd , &allset);
    while(1)
    {
        rset = allset;
        /*得到当前可读的文件描述符数*/
        nready = select(maxfd+1 , &rset , nullptr , nullptr , 0);
        /*测试listenfd是否在rset描述符集中*/
        if(FD_ISSET(listenfd , &rset))
        {
            /*接收客户端的请求*/
            clilen = sizeof(cliaddr);
            if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
            {
                perror("accept error.\n");
                exit(1);
            }
            printf("ip: %s连接到服务器\nsocketfd:%d\n", inet_ntoa(cliaddr.sin_addr),connfd);
            /*查找空闲位置，设置客户链接描述符*/
            for(i=0; i<FD_SETSIZE; ++i)
            {
                if(client_sockfd[i] < 0)
                {
                    client_sockfd[i] = connfd; /*将处理该客户端的链接套接字设置在该位置*/
                    break;
                }
            }
            if(i == FD_SETSIZE)
            {
                perror("too many connection.\n");
                exit(1);
            }
            /* 将来自客户的连接connfd加入描述符集 */
            FD_SET(connfd , &allset);
            /*新的连接描述符 -- for select*/
            if(connfd > maxfd)
                maxfd = connfd;
            /*max index in client_sockfd[]*/
            if(i > maxi)
                maxi = i;
            /*no more readable descriptors*/
            if(--nready <= 0)
                continue;
        }
        /*接下来逐个处理连接描述符*/
        for(i=0 ; i<=maxi ; ++i)
        {
            if((sockfd = client_sockfd[i]) < 0)
                continue;
            if(FD_ISSET(sockfd , &rset))
            {
                /*如果当前没有可以读的套接字，退出循环*/
                if(--nready < 0)
                    break;
                idindex = i;
                pthread_create(&pid, nullptr, ClientThread, (void *) &sockfd);
            }
            /*清除处理完的链接描述符*/
            FD_CLR(sockfd , &allset);
            //client_sockfd[i] = -1;
        }
    }
    close(listenfd);
    return 0;
}

/*处理客户请求的线程*/
void *ClientThread(void *argc)
{
    int this_index = idindex;
    int sockfd , ret , n;
    Message message;
    char buf[MAX_LINE];
    sockfd = client_sockfd[this_index];
    memset(buf , 0 , MAX_LINE);
    memset(&message , 0 , sizeof(message));
    n = recv(sockfd , buf , sizeof(buf) , 0);
    if(n <= 0)
    {
        //关闭当前描述符，并清空描述符数组
        close(sockfd);
        client_sockfd[this_index] = -1;
        printf("ip:%s退出！\n", inet_ntoa(message.sendAddr.sin_addr));
        return nullptr;
    }
    else{
        memcpy(&message , buf , sizeof(buf));
        /*为每个客户操作链接创建一个线程*/
        switch(message.msgType)
        {
            case LOGIN:
            {
                ret = loginIn(&message , sockfd);
                memset(&message , 0 , sizeof(message));
                message.msgType = RESULT;
                message.msgRet = ret;
                strcpy(message.content , Return_state(ret));
                memset(buf , 0 , MAX_LINE);
                memcpy(buf , &message , sizeof(message));
                send(sockfd , buf , sizeof(buf) , 0);
                printf("登录：%s\n", Return_state(ret));
                if(ret == SUCCESS)
                    SelectChat(&sockfd);
                break;
            }
            default:
                printf("unknown operation.\n");
                break;
        }
    }
    close(sockfd);
    client_sockfd[this_index] = -1;
    return nullptr;
}
void copyUser(User *user1 , User *user2)
{
    strcpy((*user1).userName , (*user2).userName);

    (*user1).userAddr = (*user2).userAddr;
    (*user1).sockfd = (*user2).sockfd;
}

UserList* insertUser(UserList *list , User *user)
{
    UserList *node = (UserList *)calloc(1, sizeof(UserList));
    copyUser(&(node->user) , user);
    node->next = nullptr;
    if(list == nullptr)
        list = node;
    else{
        UserList *p = list;
        while(p->next != nullptr)
        {
            p = p->next;
        }
        p->next = node;
    }
    printf("新增用户:%s！\n",user->userName);
    return list;
}
int isUser(UserList *list , User *user)
{
    UserList *p = list , *pre = p;
    while(p!= nullptr && strcmp(p->user.userName , (*user).userName) != 0)
    {
        pre = p;
        p = p->next;
    }
    if(p == nullptr)
        return 0;
    return 1;
}
void deleteUser(UserList *list , User *user)
{
    if(list == nullptr)
        return;
    UserList *p = list , *pre = p;
    while(p!= nullptr && strcmp(p->user.userName , (*user).userName) != 0)
    {
        pre = p;
        p = p->next;
    }
    if(p == nullptr)
        return ;
    else if(p == list)
    {
        list = list->next;
    }
    else if(p->next == nullptr)
    {
        pre->next = nullptr;
    }
    else
    {
        pre->next = p->next;
    }
    free(p);
    p = nullptr;
}
char *Return_state(int state)
{
    switch(state)
    {
        case SUCCESS: 
            return "成功！\n";
            break;
        case DUPLICATEID: //重复的用户名
            return "用户名重复！\n";
            break;
        default:
            return "未知操作结果！\n";
            break;
    }
};
int loginIn(Message *msg , int sockfd)
{
    int ret;
    User user;
    char buf[MAX_LINE];
    recv(sockfd , buf , sizeof(user) , 0);
    memset(&user , 0 , sizeof(user));
    memcpy(&user , buf , sizeof(user));
    user.userAddr = (*msg).sendAddr;
    user.sockfd = sockfd;
    if(isUser(userList , &user) == 1)
        return DUPLICATEID;
    else{
        ret = SUCCESS;
        /*如果登陆操作成功，添加到在线用户链表*/
        userList = insertUser(userList , &user);
        return ret;
    }
}
int Chat(Message *msg, int sockfd)
{
    UserList *p;
    int ret;
    char buf[MAX_LINE];
    Message message;
    memset(&message , 0 , sizeof(message));
    strcpy(message.sendName , (*msg).sendName);
    message.msgType = (*msg).msgType;
    /*查看在线用户*/
    p = userList;
    strcpy(message.content , (*msg).content);
    strcpy(message.msgTime , (*msg).msgTime);
    printf("%s\n",message.content);
    while(p!= nullptr)
    {
        if(strcmp((p->user).userName , message.sendName) != 0)
        {
            memset(buf , 0 , MAX_LINE);
            memcpy(buf , &message , sizeof(message));
            send((p->user).sockfd , buf , sizeof(buf) , 0);
        }
        p = p->next;
    }
    return SUCCESS;
}
int viewUserList(Message *msg , int sockfd)
{
    UserList *p;
    int ret;
    char buf[MAX_LINE];
    Message message;
    memset(&message , 0 , sizeof(message));
    strcpy(message.sendName , (*msg).sendName);
    message.msgType = (*msg).msgType;
    p = userList;
    strcpy(message.content , "");
    while(p!= nullptr)
    {
        strcat(message.content , "\t");
        strcat(message.content , (p->user).userName);
        p = p->next;
    }
    memset(buf , 0 , MAX_LINE);
    memcpy(buf , &message , sizeof(message));
    send(sockfd , buf , sizeof(buf) , 0);
    printf("查看在线用户：%s\n", message.content);
    return SUCCESS;
}
void SelectChat(int *fd)
{
    int n,ret,sockfd;
    User user;
    char buf[MAX_LINE];
    memset(buf , 0 , MAX_LINE);
    Message message;
    memset(&message , 0 , sizeof(message));
    sockfd = *fd;
    while(1)
    {
        n = recv(sockfd , buf , sizeof(buf) , 0);
        if(n == 0)
        {
            close(sockfd);
            return ;
        }
        else{
            memcpy(&message , buf , sizeof(buf));
            switch(message.msgType)
            {
                case CHAT:
                {
                    printf("%s发送：\n", message.sendName);
                    ret = Chat(&message, sockfd);
                    printf("群聊：%s\n", Return_state(ret));
                    break;
                }
                case VIEW_USER_LIST:
                {
                    printf("%s查看在线用户请求！\n", message.sendName);
                    ret = viewUserList(&message , sockfd);
                    printf("查看在线用户：%s\n", Return_state(ret));
                    break;
                }
                case EXIT:
                {
                    printf("用户%s退出聊天室！\n", message.sendName);
                    memset(&user , 0 , sizeof(user));
                    strcpy(user.userName , message.sendName);
                    deleteUser(userList , &user);
                    close(sockfd);
                    return;
                }
                default:
                    break;
            }
        }
    }
    return ;
}