//
// Created by kervias on 20-8-17.
//
#include "socket_tcp.h"

bool
init_socket_tcp_cb(
    SOCKET_TCP_CB *socket_tcp_cb,
    IPC_SEND_OR_RECEIVE type,
    char *ip_send,
    size_t ip_send_len,
    unsigned short port_send,
    char *ip_receive,
    size_t ip_receive_len,
    unsigned short port_receive
)
{
    if(socket_tcp_cb->ipc_cb.success == 666)
    {
        socket_tcp_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    socket_tcp_cb->ipc_cb.method = IPC_SOCKET_TCP;
    socket_tcp_cb->ipc_cb.type = type;

    struct in_addr addr_send;
    struct in_addr addr_receive;


    if(socket_tcp_cb->ipc_cb.type == IPC_MSG_SEND)
    {
        if(inet_aton(ip_send, &addr_send) == 0 || inet_aton(ip_receive, &addr_receive) == 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -31; //ip地址非法
            return false;
        }
        socket_tcp_cb->sock_addr_send.sin_family = AF_INET;
        socket_tcp_cb->sock_addr_send.sin_port = htons(port_send);
        socket_tcp_cb->sock_addr_send.sin_addr =  addr_send;
        socket_tcp_cb->sock_addr_receive.sin_family = AF_INET;
        socket_tcp_cb->sock_addr_receive.sin_port = htons(port_receive);
        socket_tcp_cb->sock_addr_receive.sin_addr =  addr_receive;
        if((socket_tcp_cb->socket_fd = socket(AF_INET, SOCK_STREAM,0)) < 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -32; //创建套接字失败
            return false;
        }

        if(bind(socket_tcp_cb->socket_fd, (struct sockaddr *)&socket_tcp_cb->sock_addr_send, sizeof(struct sockaddr_in)) != 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -33; //套接字绑定失败
            close(socket_tcp_cb->socket_fd); // --- 新增　---
            //printf("[errno]:%d\n", errno);
            return false;
        }

        if(connect(socket_tcp_cb->socket_fd, (struct sockaddr *)&socket_tcp_cb->sock_addr_receive, sizeof(struct sockaddr_in)) != 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -34; //套接字连接失败
            close(socket_tcp_cb->socket_fd);
            return false;
        }
        socket_tcp_cb->ipc_cb.error_code = 0;
        socket_tcp_cb->ipc_cb.success = 666;
        return true;
    }
    else{
        if(inet_aton(ip_receive, &addr_receive) == 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -31; //ip地址非法
            return false;
        }
        socket_tcp_cb->sock_addr_receive.sin_family = AF_INET;
        socket_tcp_cb->sock_addr_receive.sin_port = htons(port_receive);
        socket_tcp_cb->sock_addr_receive.sin_addr =  addr_receive;
        if((socket_tcp_cb->socket_listen = socket(AF_INET, SOCK_STREAM,0)) < 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -32; //创建套接字失败
            return false;
        }

        if(bind(socket_tcp_cb->socket_listen, (struct sockaddr *)&socket_tcp_cb->sock_addr_receive, sizeof(struct sockaddr_in)) != 0)
        {
            socket_tcp_cb->ipc_cb.error_code = -33; //套接字绑定失败
            close(socket_tcp_cb->socket_listen);
            //printf("[errno]:%d\n", errno);
            return false;
        }
        socket_tcp_cb->ipc_cb.error_code = 0;
        socket_tcp_cb->ipc_cb.success = 665;
        return true;
    }
}


bool socket_tcp_accept(SOCKET_TCP_CB *socket_tcp_cb)
{
    if(socket_tcp_cb->ipc_cb.success != 665)
    {
        socket_tcp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(socket_tcp_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        socket_tcp_cb->ipc_cb.error_code = -35; // ipc_cb通信类型不相符
        return false;
    }

    int t = listen(socket_tcp_cb->socket_listen, 1);
    if(t < 0)
    {
        socket_tcp_cb->ipc_cb.error_code = -36; // listen失败
        close(socket_tcp_cb->socket_listen);
        //close(socket_tcp_cb->socket_fd);//新增
        socket_tcp_cb->ipc_cb.success = 0;
        return false;
    }
    socklen_t tt=1;
    socket_tcp_cb->socket_fd = accept(socket_tcp_cb->socket_listen, (struct sockaddr *)&socket_tcp_cb->sock_addr_send, &tt);
    if(socket_tcp_cb->socket_fd < 0)
    {
        //printf("[errno]:%d\n", errno);
        socket_tcp_cb->ipc_cb.error_code = -37; // accept失败
        close(socket_tcp_cb->socket_listen);
        //close(socket_tcp_cb->socket_fd);//新增
        socket_tcp_cb->ipc_cb.success = 0;
        return false;
    }
//    close(socket_tcp_cb->socket_listen);
   // char *addr;

  //  addr = inet_ntoa(socket_tcp_cb->sock_addr_send.sin_addr);
    //printf("套接字accept: %s\n", addr);
    socket_tcp_cb->ipc_cb.error_code = 0;
    socket_tcp_cb->ipc_cb.success = 666;
    return true;
}



bool close_socket_tcp_cb(SOCKET_TCP_CB *socket_tcp_cb)
{
    if(socket_tcp_cb->ipc_cb.success != 666)
    {
        socket_tcp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(socket_tcp_cb->ipc_cb.type == IPC_MSG_RECEIVE)
    {
        close(socket_tcp_cb->socket_listen);
//        shutdown(socket_tcp_cb->socket_listen,SHUT_RDWR);
    }

    int t = shutdown(socket_tcp_cb->socket_fd,SHUT_RDWR);
//    if(t < 0)
//    {
//        //printf("shutdown失败");
//    }
    close(socket_tcp_cb->socket_fd);


    //printf("sss\n");
    socket_tcp_cb->ipc_cb.success = 0;
    socket_tcp_cb->ipc_cb.error_code = 0;
    return true;
}


bool socket_tcp_send_msg(SOCKET_TCP_CB *socket_tcp_cb, SOCKET_TCP_MSG *socket_tcp_msg)
{
    if(socket_tcp_cb->ipc_cb.success != 666)
    {
        socket_tcp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(socket_tcp_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        socket_tcp_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }

    int len_left = sizeof(SOCKET_TCP_MSG);
    socket_tcp_msg->msg[socket_tcp_msg->len] = '\0';
    char *ptr = (char *)socket_tcp_msg;
    int len_written = 0;
    while(len_left > 0)
    {
        if((len_written = write(socket_tcp_cb->socket_fd, ptr, len_left)) <= 0)
        {
            if(len_written < 0 && errno == EINTR)
                len_written = 0;
            else
            {
                socket_tcp_cb->ipc_cb.error_code = -38;// accept失败
               // close(socket_tcp_cb->socket_fd);
                socket_tcp_cb->ipc_cb.success = 0;
                return false;
            }
        }
        len_left -= len_written;
        ptr += len_written;
    }
    socket_tcp_cb->ipc_cb.error_code = 0;
    return true;
}


bool socket_tcp_receive_msg(SOCKET_TCP_CB *socket_tcp_cb, SOCKET_TCP_MSG *socket_tcp_msg)
{
    if(socket_tcp_cb->ipc_cb.success != 666)
    {
        socket_tcp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(socket_tcp_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        socket_tcp_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }

    int len_left = sizeof(SOCKET_TCP_MSG);
    char *ptr = (char *)socket_tcp_msg;
    int len_read = 0;
    while(len_left > 0)
    {
        if((len_read = read(socket_tcp_cb->socket_fd, ptr, len_left)) <= 0)
        {
            if(errno == EINTR)
            {
                //printf("receive EINTR");
                len_read = 0;
            }
            else if(errno == 0) //正常关闭
            {
                socket_tcp_cb->ipc_cb.error_code = 0;
                return false;
            }
            else
            {
                socket_tcp_cb->ipc_cb.error_code = -38;// accept失败
                //printf("[errno]:%d\n", errno);
               // close(socket_tcp_cb->socket_fd);//新增
                //close(socket_tcp_cb->socket_listen);
                socket_tcp_cb->ipc_cb.success = 0;
                return false;
            }

        }
        len_left -= len_read;
        ptr += len_read;
    }
    socket_tcp_cb->ipc_cb.error_code = 0;
    return true;
}

int socket_tcp_get_errcode(SOCKET_TCP_CB * socket_tcp_cb)
{
    return socket_tcp_cb->ipc_cb.error_code;
}

