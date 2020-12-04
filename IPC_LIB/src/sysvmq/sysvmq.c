//
// Created by kervias on 20-8-11.
//
#include "sysvmq.h"

bool init_sysv_mq_cb(SYSV_MQ_CB *sysv_mq_cb, IPC_SEND_OR_RECEIVE type, char *path, size_t len)
{
    if(sysv_mq_cb->ipc_cb.success == 666)
    {
        sysv_mq_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    sysv_mq_cb->ipc_cb.method = IPC_SYSTEMV_MQUEUE;
    sysv_mq_cb->ipc_cb.type = type;
    memcpy(sysv_mq_cb->path, path, len);
    sysv_mq_cb->path[len] = '\0';
    ////printf("--%s--\n", sysv_mq_cb->path);

    //生成IPC键
    key_t key = ftok(sysv_mq_cb->path, 0);
    
    sysv_mq_cb->msqid = msgget(key, IPC_CREAT | 0666);
    
    if(sysv_mq_cb->msqid < 0)
    {
        sysv_mq_cb->ipc_cb.error_code = -2; //打开或创建一个消息队列失败
        //printf("[errno]:%d\n", errno);
        return false;
    }
    
    sysv_mq_cb->ipc_cb.error_code = 0;
    sysv_mq_cb->ipc_cb.success = 666;
    return true;
}

bool close_sysv_mq_cb(SYSV_MQ_CB *sysv_mq_cb)
{
    if(sysv_mq_cb->ipc_cb.success != 666)
    {
        sysv_mq_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }

    //msgctl(sysv_mq_cb->msqid, IPC_RMID, NULL);
    
    sysv_mq_cb->ipc_cb.success = 0;
    sysv_mq_cb->ipc_cb.error_code = 0;
    return true;
}

bool sysv_mq_send_msg(SYSV_MQ_CB *sysv_mq_cb, SYSV_MQ_MSG *sysv_mq_msg)
{
    if(sysv_mq_cb->ipc_cb.success != 666)
    {
        sysv_mq_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(sysv_mq_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        sysv_mq_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }
    if(sysv_mq_msg->mtype <= 0)
    {
        sysv_mq_cb->ipc_cb.error_code = -7; // 消息类型应为>0的数
        return false;
    }
    if(msgsnd(sysv_mq_cb->msqid, sysv_mq_msg, sizeof(SYSV_MQ_MSG) - sizeof(long), IPC_NOWAIT) < 0)
    {
        sysv_mq_cb->ipc_cb.error_code = -4; //发送消息失败
        //printf("[errno]:%d\n", errno);
        return false;
    }

    sysv_mq_cb->ipc_cb.error_code = 0;
    return true;
}

int sysv_mq_get_errcode(SYSV_MQ_CB *sysv_mq_cb)
{
    return sysv_mq_cb->ipc_cb.error_code;
}

bool sysv_mq_receive_msg(SYSV_MQ_CB *sysv_mq_cb, SYSV_MQ_MSG *sysv_mq_msg)
{
    if(sysv_mq_cb->ipc_cb.success != 666)
    {
        sysv_mq_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(sysv_mq_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        sysv_mq_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }
    int t = msgrcv(sysv_mq_cb->msqid, sysv_mq_msg, sizeof(SYSV_MQ_MSG),0,0);
    if(t < 0)
    {
        sysv_mq_cb->ipc_cb.error_code = -6; // 接受信息失败
        //printf("[errno]:%d\n", errno);
        return false;
    }
    sysv_mq_msg->msg[sysv_mq_msg->len] = '\0';
    //printf("接受信息字节数: %d\n", t);
    //printf("%d %s\n", sysv_mq_msg->len, sysv_mq_msg->msg);

    sysv_mq_cb->ipc_cb.error_code = 0;
    return true;
}


