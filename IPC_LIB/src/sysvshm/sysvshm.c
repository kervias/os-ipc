//
// Created by kervias on 20-8-12.
//

#include "sysvshm.h"

bool init_sysv_shm_cb(
        SYSV_SHM_CB *sysv_shm_cb,
        IPC_SEND_OR_RECEIVE type,
        char *path,
        size_t len,
        char *path_sem_wr,
        size_t len_sem_wr,
        char *path_sem_rd,
        size_t len_sem_rd
)
{
    if(sysv_shm_cb->ipc_cb.success == 666)
    {
        sysv_shm_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    sysv_shm_cb->ipc_cb.method = IPC_SYSTEMV_SHM;
    sysv_shm_cb->ipc_cb.type = type;

    memcpy(sysv_shm_cb->path, path, len);
    memcpy(sysv_shm_cb->path_sem_wr,path_sem_wr, len_sem_wr);
    memcpy(sysv_shm_cb->path_sem_rd,path_sem_rd, len_sem_rd);
    sysv_shm_cb->path[len] = '\0';
    sysv_shm_cb->path_sem_wr[len_sem_wr] = '\0';
    sysv_shm_cb->path_sem_rd[len_sem_rd] = '\0';

    ////printf("--%s--\n", sysv_shm_cb->path);

    key_t key = ftok(sysv_shm_cb->path, 1);
    int shmid = shmget(key, sizeof(SYSV_SHM_MSG), SVMSG_MODE | IPC_CREAT);
    if(shmid < 0)
    {
        sysv_shm_cb->ipc_cb.error_code = -20; //打开或创建sysv_shm失败
        //printf("[errno]:%d\n", errno);
        return false;
    }

    sysv_shm_cb->ptr  = shmat(shmid, NULL, 0);
    if(*(int*)sysv_shm_cb->ptr == -1)
    {
        sysv_shm_cb->ipc_cb.error_code = -21; //映射失败
        //printf("[errno]:%d\n", errno);
        return false;
    }


    sysv_shm_cb->mutex_wr = sem_open(sysv_shm_cb->path_sem_wr, O_CREAT, FILE_MODE, 1);
    if(sysv_shm_cb->mutex_wr == SEM_FAILED)
    {
        sysv_shm_cb->ipc_cb.error_code = -22; //sysv有名信号量创建失败映射失败
        shmdt(sysv_shm_cb->path);
        //printf("[errno]:%d\n", errno);
        return false;
    }
    sysv_shm_cb->mutex_rd = sem_open(sysv_shm_cb->path_sem_rd, O_CREAT, FILE_MODE, 0);
    if(sysv_shm_cb->mutex_rd == SEM_FAILED)
    {
        sysv_shm_cb->ipc_cb.error_code = -23; //sysv有名信号量创建失败映射失败
        shmdt(sysv_shm_cb->path);
        sem_close(sysv_shm_cb->mutex_wr);
        //printf("[errno]:%d\n", errno);
        return false;
    }

    sysv_shm_cb->ipc_cb.error_code = 0;
    sysv_shm_cb->ipc_cb.success = 666;
    return true;
}

bool close_sysv_shm_cb(SYSV_SHM_CB *sysv_shm_cb)
{
    if(sysv_shm_cb->ipc_cb.success != 666)
    {
        sysv_shm_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }


    sem_close(sysv_shm_cb->mutex_wr);
    sem_close(sysv_shm_cb->mutex_rd);
    shmdt(sysv_shm_cb->path);
    //printf("[%s][%s][%s]", sysv_shm_cb->path_sem_wr,sysv_shm_cb->path_sem_rd, sysv_shm_cb->path);
    //sem_unlink(sysv_shm_cb->path_sem_wr);
    //sem_unlink(sysv_shm_cb->path_sem_rd);

    sysv_shm_cb->ipc_cb.success = 0;
    sysv_shm_cb->ipc_cb.error_code = 0;
    return true;
}

bool sysv_shm_send_msg(SYSV_SHM_CB *sysv_shm_cb, SYSV_SHM_MSG *sysv_shm_msg)
{
    if(sysv_shm_cb->ipc_cb.success != 666)
    {
        sysv_shm_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(sysv_shm_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        sysv_shm_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }

    int times = 5; //尝试次数
    bool b = false;
    int t1,t2;
    while(times--)
    {
        int t1 = sem_trywait(sysv_shm_cb->mutex_wr);
        if(t1 < 0)
        {
            if(errno == EAGAIN)
            {
                continue;
            }else{
                sysv_shm_cb->ipc_cb.error_code = -14; //发送报错
                //printf("[errno]:%d\n", errno);
                return false;
            }
        }else{
            sysv_shm_cb->ptr->len = sysv_shm_msg->len;
            memcpy(sysv_shm_cb->ptr->msg, sysv_shm_msg->msg, sysv_shm_msg->len);
            sysv_shm_cb->ptr->msg[sysv_shm_msg->len] = '\0';
            t2 =  sem_post(sysv_shm_cb->mutex_rd);
            if(t2 < 0)
            {
                sysv_shm_cb->ipc_cb.error_code = -15; //发送报错
                //printf("[errno]:%d\n", errno);
                return false;
            }
            b = true;
            break;
        }
    }
    if(!b)
    {
        sysv_shm_cb->ipc_cb.error_code = 10; //消息没有被读取
        return false;
    }

    sysv_shm_cb->ipc_cb.error_code = 0;
    return true;
}

int sysv_shm_get_errcode(SYSV_SHM_CB *sysv_shm_cb)
{
    return sysv_shm_cb->ipc_cb.error_code;
}

bool sysv_shm_receive_msg(SYSV_SHM_CB *sysv_shm_cb, SYSV_SHM_MSG *sysv_shm_msg)
{
    if(sysv_shm_cb->ipc_cb.success != 666)
    {
        sysv_shm_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(sysv_shm_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        sysv_shm_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }

    int t1 = sem_wait(sysv_shm_cb->mutex_rd);
    if(t1 < 0)
    {
        sysv_shm_cb->ipc_cb.error_code = -16;
        //printf("[errno]:%d\n", errno);
        return false;
    }

    sysv_shm_msg->len = sysv_shm_cb->ptr->len;
    memcpy(sysv_shm_msg->msg, sysv_shm_cb->ptr->msg, sysv_shm_msg->len);
    sysv_shm_msg->msg[sysv_shm_msg->len] = '\0';
    t1 = sem_post(sysv_shm_cb->mutex_wr);
    if(t1 < 0)
    {
        sysv_shm_cb->ipc_cb.error_code = -17;
        //printf("[errno]:%d\n", errno);
        return false;
    }

//    //printf("接受信息字节数: %d\n", t);
//    //printf("%d %s\n", sysv_shm_msg->len, sysv_shm_msg->msg);

    sysv_shm_cb->ipc_cb.error_code = 0;
    return true;
}



