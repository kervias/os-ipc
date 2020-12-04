//
// Created by kervias on 20-8-17.
//
#include "socket_udp.h"

bool
init_socket_udp_cb(
        SOCKET_UDP_CB *socket_udp_cb,
        IPC_SEND_OR_RECEIVE type,
        char *ip_send,
        size_t ip_send_len,
        unsigned short port_send,
        char *ip_receive,
        size_t ip_receive_len,
        unsigned short port_receive
)
{
    if(socket_udp_cb->ipc_cb.success == 666)
    {
        socket_udp_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    socket_udp_cb->ipc_cb.method = IPC_SOCKET_UDP;
    socket_udp_cb->ipc_cb.type = type;

    struct in_addr addr_send;
    struct in_addr addr_receive;

    if(inet_aton(ip_receive, &addr_receive) == 0)
    {
        socket_udp_cb->ipc_cb.error_code = -31; //ip地址非法
        return false;
    }

    if(socket_udp_cb->ipc_cb.type == IPC_MSG_SEND)
    {
        if(inet_aton(ip_send, &addr_send) == 0)
        {
            socket_udp_cb->ipc_cb.error_code = -31; //ip地址非法
            return false;
        }
        socket_udp_cb->sock_addr_send.sin_family = AF_INET;
        socket_udp_cb->sock_addr_send.sin_port = htons(port_send);
        socket_udp_cb->sock_addr_send.sin_addr =  addr_send;
    }

    socket_udp_cb->sock_addr_receive.sin_family = AF_INET;
    socket_udp_cb->sock_addr_receive.sin_port = htons(port_receive);
    socket_udp_cb->sock_addr_receive.sin_addr =  addr_receive;

    if((socket_udp_cb->socket_fd = socket(AF_INET, SOCK_DGRAM,0)) < 0)
    {
        socket_udp_cb->ipc_cb.error_code = -32; //创建套接字失败
        return false;
    }

    if(socket_udp_cb->ipc_cb.type == IPC_MSG_SEND)
    {
        if(bind(socket_udp_cb->socket_fd, (struct sockaddr *)&socket_udp_cb->sock_addr_send, sizeof(struct sockaddr_in)) != 0)
        {
            socket_udp_cb->ipc_cb.error_code = -33; //套接字绑定失败
            close(socket_udp_cb->socket_fd);
            //printf("[errno]:%d\n", errno);
            return false;
        }
    }
    else{
        if(bind(socket_udp_cb->socket_fd, (struct sockaddr *)&socket_udp_cb->sock_addr_receive, sizeof(struct sockaddr_in)) != 0)
        {
            socket_udp_cb->ipc_cb.error_code = -33; //套接字绑定失败
            close(socket_udp_cb->socket_fd);
            //printf("[errno]:%d\n", errno);
            return false;
        }
    }
    socket_udp_cb->ipc_cb.error_code = 0;
    socket_udp_cb->ipc_cb.success = 666;
    return true;
}



bool close_socket_udp_cb(SOCKET_UDP_CB *socket_udp_cb)
{
    if(socket_udp_cb->ipc_cb.success != 666)
    {
        socket_udp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }

    close(socket_udp_cb->socket_fd);

    //printf("sss");
    socket_udp_cb->ipc_cb.success = 0;
    socket_udp_cb->ipc_cb.error_code = 0;
    return true;
}


bool socket_udp_send_msg(SOCKET_UDP_CB *socket_udp_cb, SOCKET_UDP_MSG *socket_udp_msg)
{
    if(socket_udp_cb->ipc_cb.success != 666)
    {
        socket_udp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(socket_udp_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        socket_udp_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }

    int len_left = sizeof(SOCKET_UDP_MSG);
    socket_udp_msg->msg[socket_udp_msg->len] = '\0';
    char *ptr = (char *)socket_udp_msg;
    int len_written = 0;
    while(len_left > 0)
    {
        if((len_written = sendto(socket_udp_cb->socket_fd, ptr, len_left, 0,  (struct sockaddr *)&socket_udp_cb->sock_addr_receive, sizeof(struct sockaddr_in) )) <= 0)
        {
            if(len_written < 0 && errno == EINTR)
                len_written = 0;
            else
            {
                socket_udp_cb->ipc_cb.error_code = -38;// accept失败
                //printf("[errno]:%d\n", errno);
               // close(socket_udp_cb->socket_fd);
               // socket_udp_cb->ipc_cb.success = 0;
                return false;
            }
        }
        len_left -= len_written;
        ptr += len_written;
    }
    socket_udp_cb->ipc_cb.error_code = 0;
    return true;
}


bool socket_udp_receive_msg(SOCKET_UDP_CB *socket_udp_cb, SOCKET_UDP_MSG *socket_udp_msg)
{
    if(socket_udp_cb->ipc_cb.success != 666)
    {
        socket_udp_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(socket_udp_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        socket_udp_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }

    int len_left = sizeof(SOCKET_UDP_MSG);
    char *ptr = (char *)socket_udp_msg;
    int len_read = 0;
    int t = 0;
    while(len_left > 0)
    {
        if((len_read = recvfrom(socket_udp_cb->socket_fd, ptr, len_left, 0,  (struct sockaddr *)&socket_udp_cb->sock_addr_receive, &t)) <= 0)
        {
            if(errno == EINTR)
            {
                //printf("receive EINTR");
                len_read = 0;
            }
            else if(errno == 0) //正常关闭
            {
                socket_udp_cb->ipc_cb.error_code = 0;
                return false;
            }
            else
            {
                socket_udp_cb->ipc_cb.error_code = -38;// accept失败
                //printf("[errno]:%d\n", errno);
                //close(socket_udp_cb->socket_fd);
                //socket_udp_cb->ipc_cb.success = 0;
                return false;
            }
        }
        len_left -= len_read;
        ptr += len_read;
    }
    //printf("[addr]: %s", inet_ntoa(socket_udp_cb->sock_addr_send.sin_addr));
    socket_udp_cb->ipc_cb.error_code = 0;
    return true;
}

int socket_udp_get_errcode(SOCKET_UDP_CB * socket_udp_cb)
{
    return socket_udp_cb->ipc_cb.error_code;
}

