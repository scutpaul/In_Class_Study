/*************************************
 * creator:scutpaul
 * socket_client
 * use: select,socket,pthread
 *
 *************************************/
#include"conf.h"
int sockfd;
User user;
Message send_message;
Message recv_message;
char send_buf[MAX_LINE];
char recv_buf[MAX_LINE];

int HelloUI();
void OpUI();
int LoginIn();
void Operate();
void *RecvMsg(void *agrv);
int HelloUI()
{

    printf("-------------------------------------\n");
    printf("        201530311042 陈浩鑫        \n");
    printf("          欢迎进入群聊天室～	      \n");
    printf("	         1.登陆			      \n");
    printf("	         2.退出			      \n");
    printf("-------------------------------------\n\n");
}
void OpUI()
{
    printf("------------------------------------------\n");
    printf("你好，%s                       \n", user.userName);
    printf("选择操作：1. 查看在线用户  2.群聊   3.退出\n");
    printf("------------------------------------------\n\n");
}


int main(int argc , char *argv[])
{
    int  choice ;

    struct sockaddr_in6 servaddr;
    char * connect_ip;
    if(argc == 2){
        strcpy(connect_ip,argv[1]);
    }else if(argc == 1){
        //connect_ip="2001:470:18:223::2";
        //connect_ip = "139.199.189.186";
        //connect_ip = "120.78.206.129";
        connect_ip = "::1";
        printf("set connect default ipv6: #::1\n");
    }else {
        perror("usage:socket_client <IP>");
        exit(1);
    }
    while(1)
    {
        if((sockfd = socket(AF_INET6 , SOCK_STREAM , 0)) == -1)
        {
            perror("built socket error");
            exit(1);
        }
        bzero(&servaddr , sizeof(servaddr));		/*清空地址结构*/
        servaddr.sin6_family = AF_INET6;				/*使用IPV4通信域*/
        servaddr.sin6_port = htons(PORT);			/*端口号转换为网络字节序*/
        if(inet_pton(AF_INET6 , connect_ip , &servaddr.sin6_addr) < 0)
        {
            printf("inet_pton error for %s\n",connect_ip);
            exit(1);
        }
        if( connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
        {
            perror("connect error");
            exit(1);
        }
        HelloUI();
        setbuf(stdin, nullptr);
        scanf("%d",&choice);
        setbuf(stdin, nullptr);
        while(choice != 1 && choice != 2 )
        {
            printf("请重新输入！\n");
            setbuf(stdin, nullptr);
            scanf("%d",&choice);
            setbuf(stdin, nullptr);
        }

        switch(choice)
        {
            case LOGIN:
                memset(&send_message , 0 , sizeof(send_message));
                memset(send_buf , 0 , MAX_LINE);
                send_message.msgType = LOGIN;
                send_message.msgRet = 2;
                strcpy(send_message.content , "--");
                //printf("hellow%d",send_message.msgType);
                send_message.sendAddr = servaddr;

                //printf("%d", sizeof(send_message));
                //fflush(stdout);
                memcpy(send_buf , &send_message , sizeof(send_buf));
                //printf("1");
                //fflush(stdout);
                send(sockfd , send_buf , sizeof(send_buf) , 0);
                LoginIn();
                break;
            case EXIT:
                close(sockfd);
                printf("退出聊天室!\n");
                exit(0);	/*用户退出*/
                break;
            default:
                printf("unknown operation.\n");
                break;
        }
    }
    close(sockfd);
    return 0;
}
int LoginIn()
{
    /*获取用户输入*/
    //printf("0");
    //fflush(stdout);
    strcpy(user.userName , "***");
    printf("请输入用户名：\n");
    memset(user.userName , 0 , sizeof(user.userName));
    scanf("%s" , user.userName);
    //printf("user.UserName = %s\n" , user.userName);
    memset(send_buf , 0 , MAX_LINE);
    memcpy(send_buf , &user , sizeof(user));
    send(sockfd , send_buf , sizeof(send_buf) , 0);
    memset(recv_buf , 0 , MAX_LINE);
    recv(sockfd , recv_buf , sizeof(recv_buf) , 0);
    memset(&recv_message , 0 , sizeof(recv_message));
    memcpy(&recv_message , recv_buf , sizeof(recv_buf));
    printf("%s\n",recv_message.content);

    /*如果登陆成功，则进入聊天*/
    if(SUCCESS == recv_message.msgRet)
    {
        //printf("success");
        Operate();
    }else if(DUPLICATEID == recv_message.msgRet){
        printf("user name duplicated");
        LoginIn();
    }
    return recv_message.msgRet;
}

void *RecvMsg(void *agrv)
{
    int connfd = sockfd;
    int nRead;

    char buf[MAX_LINE] , str[MAX_LINE];
    Message message;

    printf("开始接收聊天信息:\n");
    while(1)
    {
        /*接收服务器发来的消息*/
        nRead = recv(connfd , buf , sizeof(message) , 0);
        /*recv函数返回值 <0 出错  =0 链接关闭  >0接收到的字节数*/
        if(nRead <= 0)
        {
            printf("您已经异常掉线，请重新登录！\n");
            close(connfd);
            exit(0);
        }//if

        memset(&message , 0 , sizeof(message));
        memcpy(&message , buf , sizeof(buf));

        switch(message.msgType)
        {
            case VIEW_USER_LIST:
                printf("当前在线用户有：\n %s\n", message.content);
                break;
            case CHAT:
                sprintf(str , "%s \t %s \t发送群消息： %s\n", message.sendName , message.msgTime , message.content);
                printf("\n%s\n", str);
                break;
            case RESULT:
                printf("你的操作结果：%s\n", message.content);
            default:
                break;
        }//switch
    }//while
}

void Operate()
{
    int choice , ret;
    time_t timep;
    pthread_t pid;
    ret = pthread_create(&pid, nullptr, RecvMsg, nullptr);
    if(ret != 0)
    {
        printf("软件异常，请重新登录！\n");
        memset(&send_message , 0 , sizeof(send_message));
        strcpy(send_message.sendName , user.userName);
        send_message.msgType = EXIT;
        send(sockfd , send_buf , sizeof(send_buf) , 0);
        close(sockfd);
        exit(1);
    }
    setbuf(stdin, nullptr);
    OpUI();
    while(1)
    {
        memset(&send_message , 0 , sizeof(send_message));
        strcpy(send_message.sendName , user.userName);
        memset(send_buf , 0 , MAX_LINE);
        usleep(100000);
        setbuf(stdin, nullptr);
        scanf("%d",&choice);
        while(choice != 1 && choice != 2 && choice != 3 && choice !=4 && choice != 5)
        {
            printf("未知操作，请重新输入！\n");
            setbuf(stdin, nullptr);
            scanf("%d",&choice);
            setbuf(stdin, nullptr);
        }

        switch(choice)
        {
            case 1: /*查看当前在线用户*/
                send_message.msgType = VIEW_USER_LIST;
                memcpy(send_buf , &send_message , sizeof(send_message));
                send(sockfd , send_buf , sizeof(send_buf) , 0);
                break;
            case 2: /*聊天*/
                send_message.msgType = CHAT;
                printf("请输入聊天内容：\n");
                setbuf(stdin , nullptr);
                fgets(send_message.content , MAX_LINE , stdin);
                (send_message.content)[strlen(send_message.content) - 1] = '\0';
                time(&timep);
                printf("ss");

                //printf("%d:%d", sizeof(ctime(&timep)), sizeof(send_message.msgTime));
                //fflush(stdout);
                //printf("%s",ctime(&timep));
                //strcpy(send_message.msgTime , ctime(&timep));
                memcpy(send_message.msgTime,ctime(&timep), sizeof(ctime(&timep)));

                //printf("%d:%d", sizeof(send_message), sizeof(send_buf));
                //fflush(stdout);
                memcpy(send_buf , &send_message , sizeof(send_message));
                //printf("hhh00");
                //fflush(stdout);
                send(sockfd , send_buf , sizeof(send_buf) , 0);
                printf("已发出\n");
                break;
            case 3: /*退出登陆*/
                send_message.msgType = EXIT;
                memcpy(send_buf , &send_message , sizeof(send_message));
                send(sockfd , send_buf , sizeof(send_buf) , 0);
                close(sockfd);
                exit(0);
            default: 	/*未知操作类型*/
                break;
        }
    }
}