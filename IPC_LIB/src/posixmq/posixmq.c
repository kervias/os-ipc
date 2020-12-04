//
// Created by kervias on 20-8-11.
//

#include "posixmq.h"

bool init_posix_mq_cb(POSIX_MQ_CB *posix_mq_cb, IPC_SEND_OR_RECEIVE type, char *path, size_t len)
{
    if(posix_mq_cb->ipc_cb.success == 666)
    {
        posix_mq_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    posix_mq_cb->ipc_cb.method = IPC_POSIX_MQUEUE;
    posix_mq_cb->ipc_cb.type = type;
    memcpy(posix_mq_cb->path, path, len);
    posix_mq_cb->path[len] = '\0';

    struct mq_attr attr;
    attr.mq_maxmsg = POSIX_MQ_MAX_MSG;
    attr.mq_msgsize = sizeof(POSIX_MQ_MSG);
    if(posix_mq_cb->ipc_cb.type == IPC_MSG_SEND)
    {
        posix_mq_cb->msqid = mq_open(posix_mq_cb->path, O_WRONLY | O_CREAT | O_NONBLOCK, FILE_MODE, &attr);
    }else{
        posix_mq_cb->msqid = mq_open(posix_mq_cb->path, O_RDONLY | O_CREAT, FILE_MODE, &attr);
    }

    if(posix_mq_cb->msqid < 0)
    {
        posix_mq_cb->ipc_cb.error_code = -2; //打开或创建一个消息队列失败
        //printf("[errno]:%d\n", errno);
        return false;
    }

    posix_mq_cb->ipc_cb.error_code = 0;
    posix_mq_cb->ipc_cb.success = 666;
    return true;
}

bool close_posix_mq_cb(POSIX_MQ_CB *posix_mq_cb)
{
    if(posix_mq_cb->ipc_cb.success != 666)
    {
        posix_mq_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }

    mq_close(posix_mq_cb->msqid);

    posix_mq_cb->ipc_cb.success = 0;
    posix_mq_cb->ipc_cb.error_code = 0;
    return true;
}

bool posix_mq_send_msg(POSIX_MQ_CB *posix_mq_cb, POSIX_MQ_MSG *posix_mq_msg)
{
    if(posix_mq_cb->ipc_cb.success != 666)
    {
        posix_mq_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(posix_mq_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        posix_mq_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }
    if(posix_mq_msg->mtype >= MQ_PRIO_MAX)
    {
        posix_mq_cb->ipc_cb.error_code = -8; // 消息类型大小
        return false;
    }

    if(mq_send(posix_mq_cb->msqid, (char *)posix_mq_msg, sizeof(POSIX_MQ_MSG), posix_mq_msg->mtype) < 0)
    {
        posix_mq_cb->ipc_cb.error_code = -4; //发送消息失败
        //printf("[errno]:%d\n", errno);
        return false;
    }

    posix_mq_cb->ipc_cb.error_code = 0;
    return true;
}

int posix_mq_get_errcode(POSIX_MQ_CB *posix_mq_cb)
{
    return posix_mq_cb->ipc_cb.error_code;
}

bool posix_mq_receive_msg(POSIX_MQ_CB *posix_mq_cb, POSIX_MQ_MSG *posix_mq_msg)
{
    if(posix_mq_cb->ipc_cb.success != 666)
    {
        posix_mq_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(posix_mq_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        posix_mq_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }
    unsigned int prior;
    int t = mq_receive(posix_mq_cb->msqid, (char *)posix_mq_msg, sizeof(POSIX_MQ_MSG), &prior);
    if(t < 0)
    {
        posix_mq_cb->ipc_cb.error_code = -6; // 接受信息失败
        return false;
    }
    posix_mq_msg->msg[posix_mq_msg->len] = '\0';
    //printf("接受信息字节数: %d\n", t);
    //printf("%d %s\n", posix_mq_msg->len, posix_mq_msg->msg);

    posix_mq_cb->ipc_cb.error_code = 0;
    return true;
}


