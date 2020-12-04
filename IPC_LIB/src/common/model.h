#ifndef MODEL_H
#define MODEL_H

#include <unistd.h>
//#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TRANSTO_IPC_CB(ipc_cb) (IPC_CB *)(ipc_cb) //强制转换为IPC_CB
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

typedef enum _IPC_METHOD
{
    IPC_FIFO, // 有名管道
    IPC_SOCKET_UDP, //网络SOCKET的UDP方式
    IPC_SOCKET_TCP, //网络SOKCET的TCP方式
    IPC_POSIX_MQUEUE, //POSIX消息队列
    IPC_POSIX_SHM, //POSIX共享内存
    IPC_SYSTEMV_MQUEUE, //SYSTEM V消息队列
    IPC_SYSTEMV_SHM //SYSTEM V共享内存
}IPC_METHOD;

typedef enum _IPC_SEND_OR_RECEIVE
{
    IPC_MSG_SEND,
    IPC_MSG_RECEIVE
}IPC_SEND_OR_RECEIVE;

typedef struct _IPC_CB //IPC控制块
{
    IPC_METHOD method; //　IPC类型
    int success; // 初始化是否成功 success=666时成功
    int error_code; //每次对IPC_CB操作后的error_code
    IPC_SEND_OR_RECEIVE type;
}IPC_CB;



//  -------     FIFO    -------------
#define FIFO_MSG_MAX_LEN 1024 //FIFO每次发送消息的大小限制

typedef struct _FIFO_CB
{
    IPC_CB ipc_cb; //以便强制转换
    int fd;   //管道描述符
    char fifo_path[PATH_MAX]; // 管道路径名称
}FIFO_CB;


typedef struct _FIFO_MSG
{
    unsigned int len; //实际消息大小
    char msg[FIFO_MSG_MAX_LEN]; //消息内容承载结构
}FIFO_MSG;

//  -------     SystemV MQ    -------------
#define	MSG_R	0400
#define	MSG_W	0200

#define	SVMSG_MODE	(MSG_R | MSG_W | MSG_R>>3 | MSG_R>>6)
#define SYSV_MQ_MSG_MAX_LEN 1024

typedef struct _SYSV_MQ_CB
{
    IPC_CB  ipc_cb;
    int msqid; //SystemV 消息队列ID
    char path[PATH_MAX]; //路径
}SYSV_MQ_CB;

typedef struct _SYSV_MQ_MSG
{
    long mtype;
    unsigned int len;
    char msg[SYSV_MQ_MSG_MAX_LEN];
}SYSV_MQ_MSG;


//  -------     Posix MQ    -------------
#define POSIX_MQ_MSG_SIZE 1024
#define POSIX_MQ_MAX_MSG 10
typedef struct _POSIX_MQ_CB
{
    IPC_CB ipc_cb;
    mqd_t msqid;
    char path[PATH_MAX];
}POSIX_MQ_CB;

typedef struct _POSIX_MQ_MSG
{
    unsigned int mtype;
    unsigned int len;
    char msg[POSIX_MQ_MSG_SIZE];
}POSIX_MQ_MSG;


//  -------     Posix SHM   -------------
#define POSIX_SHM_MSG_SIZE 1024

typedef struct _POSIX_SHM_MSG
{
    unsigned int len;
    char msg[POSIX_SHM_MSG_SIZE];
}POSIX_SHM_MSG;

typedef struct _POSIX_SHM_CB
{
    IPC_CB ipc_cb;
    char path[PATH_MAX]; //Poisx共享内存名
    POSIX_SHM_MSG *ptr; //共享内存映射到本进程的地址
    char path_sem_wr[PATH_MAX],path_sem_rd[PATH_MAX]; //Posix有名信号量地址
    sem_t *mutex_wr, *mutex_rd; //Posix有名信号量指针
}POSIX_SHM_CB;


//  -------     System V SHM   -------------
#define SYSV_SHM_MSG_SIZE 1024
#define	SVSHM_MODE	(SHM_R | SHM_W | SHM_R>>3 | SHM_R>>6)
typedef struct _SYSV_SHM_MSG
{
    unsigned int len;
    char msg[SYSV_SHM_MSG_SIZE];
}SYSV_SHM_MSG;

typedef struct _SYSV_SHM_CB
{
    IPC_CB ipc_cb;
    char path[PATH_MAX]; //SystemV共享内存名
    SYSV_SHM_MSG *ptr; //共享内存映射到本进程的地址

    char path_sem_wr[PATH_MAX],path_sem_rd[PATH_MAX]; //Posix有名信号量地址
    sem_t *mutex_wr, *mutex_rd; //Posix有名信号量指针
}SYSV_SHM_CB;

// -------     SOCKET TCP  -------------
#define SOCKET_TCP_MSG_SIZE 1024

typedef struct _SOCKET_TCP_MSG
{
    unsigned int len;
    char msg[SOCKET_TCP_MSG_SIZE];
}SOCKET_TCP_MSG;


typedef struct _SOCKET_TCP_CB
{
    IPC_CB ipc_cb;
    int socket_fd; //发送或接收的fd
    int socket_listen; //监听套接字
    struct sockaddr_in sock_addr_send; //发送方的IP地址
    int port_send; //发送端端口
    struct sockaddr_in sock_addr_receive; //接收方的IP地址
    int port_receive; // 接收端端口
}SOCKET_TCP_CB;

// -------     SOCKET UDP  -------------
#define SOCKET_UDP_MSG_SIZE 1024

typedef struct _SOCKET_UDP_MSG
{
    unsigned int len;
    char msg[SOCKET_UDP_MSG_SIZE];
}SOCKET_UDP_MSG;


typedef struct _SOCKET_UDP_CB
{
    IPC_CB ipc_cb;
    int socket_fd; //发送或接收的fd
    struct sockaddr_in sock_addr_send; //发送方的IP地址
    int port_send; //发送端端口
    struct sockaddr_in sock_addr_receive; //接收方的IP地址
    int port_receive; // 接收端端口
}SOCKET_UDP_CB;

#endif // MODEL_H
