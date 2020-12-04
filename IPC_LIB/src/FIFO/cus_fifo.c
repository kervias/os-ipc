//
// Created by kervias on 20-8-11.
//

#include "cus_fifo.h"


bool init_fifo_cb(FIFO_CB *fifo_cb,IPC_SEND_OR_RECEIVE type,char *fifo_path,size_t len)
{
    if(fifo_cb->ipc_cb.success == 666)
    {
        fifo_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    fifo_cb->ipc_cb.method = IPC_FIFO;
    fifo_cb->ipc_cb.type = type;
    memcpy(fifo_cb->fifo_path, fifo_path, len);
    fifo_cb->fifo_path[len] = '\0';
    ////printf("--%s--\n", fifo_cb->fifo_path);

    // 不存在则创建
    if((mkfifo(fifo_cb->fifo_path, 0777) < 0) && (errno != EEXIST))
    {
        fifo_cb->ipc_cb.error_code = -2; //创建FIFO失败
        //printf("[errno]:%d\n", errno);
        return false;
    }
    // 打开FIFO
    if(type == IPC_MSG_SEND)
    {
        fifo_cb->fd = open(fifo_cb->fifo_path, O_WRONLY | O_NONBLOCK, 0);
    }else
    {
        //printf("open 1\n");
        fifo_cb->fd = open(fifo_cb->fifo_path, O_RDWR, 0);
        //printf("open 2\n");
    }
    if(fifo_cb->fd < 0)
    {
        fifo_cb->ipc_cb.error_code = -6; //打开FIFO失败.
        //printf("[errno]:open fifo %d\n", errno);
        return false;
    }

    fifo_cb->ipc_cb.error_code = 0;
    fifo_cb->ipc_cb.success = 666;
    return true;
}


bool close_fifo_cb(FIFO_CB *fifo_cb)
{
    if(fifo_cb->ipc_cb.success != 666)
    {
        fifo_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }

    close(fifo_cb->fd);
//    unlink(fifo_cb->fifo_path);

    fifo_cb->ipc_cb.success = 0;
    fifo_cb->ipc_cb.error_code = 0;
    return true;
}


bool fifo_send_msg(FIFO_CB *fifo_cb, FIFO_MSG *fifo_msg)
{
    if(fifo_cb->ipc_cb.success != 666)
    {
        fifo_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(fifo_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        fifo_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }

    //if(write(fifo_cb->fd, "hello", sizeof("hello")) < 0)
    if(write(fifo_cb->fd, fifo_msg, sizeof(FIFO_MSG)) < 0)
    {
        fifo_cb->ipc_cb.error_code = -4; //发送消息失败
        //printf("[errno]:%d\n", errno);
        return false;
    }

    fifo_cb->ipc_cb.error_code = 0;
    return true;
}

int fifo_get_errcode(FIFO_CB *fifo_cb)
{
    return fifo_cb->ipc_cb.error_code;
}

bool fifo_receive_msg(FIFO_CB *fifo_cb, FIFO_MSG *fifo_msg)
{
    if(fifo_cb->ipc_cb.success != 666)
    {
        fifo_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(fifo_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        fifo_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }
    int t = read(fifo_cb->fd, fifo_msg, sizeof(FIFO_MSG));
    if(t < 0)
    {
        fifo_cb->ipc_cb.error_code = -6; // ipc_cb接收消息失败
        return false;
    }
    fifo_msg->msg[fifo_msg->len] = '\0';
//    //printf("接受信息字节数: %d\n", t);
//    //printf("%d %s\n", fifo_msg->len, fifo_msg->msg);

    fifo_cb->ipc_cb.error_code = 0;
    return true;
}

size_t fifo_get_pipe_buf()
{
    return PIPE_BUF;
}
