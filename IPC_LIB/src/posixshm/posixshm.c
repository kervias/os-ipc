//
// Created by kervias on 20-8-12.
//

#include "posixshm.h"

bool init_posix_shm_cb(
        POSIX_SHM_CB *posix_shm_cb, 
        IPC_SEND_OR_RECEIVE type, 
        char *path,
        size_t len,
        char *path_sem_wr,
        size_t len_sem_wr,
        char *path_sem_rd,
        size_t len_sem_rd
        )
{
    if(posix_shm_cb->ipc_cb.success == 666)
    {
        posix_shm_cb->ipc_cb.error_code = -1; //重复初始化错误
        return false;
    }
    posix_shm_cb->ipc_cb.method = IPC_POSIX_SHM;
    posix_shm_cb->ipc_cb.type = type;

    memcpy(posix_shm_cb->path, path, len);
    memcpy(posix_shm_cb->path_sem_wr,path_sem_wr, len_sem_wr);
    memcpy(posix_shm_cb->path_sem_rd,path_sem_rd, len_sem_rd);
    posix_shm_cb->path[len] = '\0';
    posix_shm_cb->path_sem_wr[len_sem_wr] = '\0';
    posix_shm_cb->path_sem_rd[len_sem_rd] = '\0';
    
    ////printf("--%s--\n", posix_shm_cb->path);

    int flags = O_CREAT | O_RDWR;
//    if(posix_shm_cb->ipc_cb.type == IPC_MSG_SEND)
//    {
//        flags |= O_WRONLY;
//    }else{
//        flags |= O_RDONLY;
//    }

    int fd = shm_open(posix_shm_cb->path, flags, FILE_MODE);
    if(fd < 0)
    {
        posix_shm_cb->ipc_cb.error_code = -10; //打开或创建posix_shm失败
        //printf("[errno]:%d\n", errno);
        return false;
    }

    int flags2 = 0;
    if(posix_shm_cb->ipc_cb.type == IPC_MSG_SEND)
        flags2 = PROT_WRITE;
    else
        flags2 = PROT_READ;

    ftruncate(fd, sizeof(POSIX_SHM_MSG)); //设置共享内存区大小

    void *pp = mmap(NULL, sizeof(POSIX_SHM_MSG), flags2, MAP_SHARED, fd,0);
    close(fd);
    if(pp == MAP_FAILED)
    {
        posix_shm_cb->ipc_cb.error_code = -11; //mmap映射失败
        //printf("[errno]:%d\n", errno);
        return false;
    }
    posix_shm_cb->ptr = (POSIX_SHM_MSG *)pp;

    posix_shm_cb->mutex_wr = sem_open(posix_shm_cb->path_sem_wr, O_CREAT, FILE_MODE, 1);
    if(posix_shm_cb->mutex_wr == SEM_FAILED)
    {
        posix_shm_cb->ipc_cb.error_code = -12; //posix有名信号量创建失败映射失败
        munmap(posix_shm_cb->ptr, sizeof(POSIX_SHM_MSG));
        //printf("[errno]:%d\n", errno);
        return false;
    }
    posix_shm_cb->mutex_rd = sem_open(posix_shm_cb->path_sem_rd, O_CREAT, FILE_MODE, 0);
    if(posix_shm_cb->mutex_rd == SEM_FAILED)
    {
        posix_shm_cb->ipc_cb.error_code = -13; //posix有名信号量创建失败映射失败
        munmap(posix_shm_cb->ptr, sizeof(POSIX_SHM_MSG));
        sem_close(posix_shm_cb->mutex_wr);
        //printf("[errno]:%d\n", errno);
        return false;
    }

    posix_shm_cb->ipc_cb.error_code = 0;
    posix_shm_cb->ipc_cb.success = 666;
    return true;
}

bool close_posix_shm_cb(POSIX_SHM_CB *posix_shm_cb)
{
    if(posix_shm_cb->ipc_cb.success != 666)
    {
        posix_shm_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }


    munmap(posix_shm_cb->ptr, sizeof(POSIX_SHM_MSG));
    sem_close(posix_shm_cb->mutex_wr);
    sem_close(posix_shm_cb->mutex_rd);

    //sem_unlink(posix_shm_cb->path_sem_wr);
    //sem_unlink(posix_shm_cb->path_sem_rd);

    posix_shm_cb->ipc_cb.success = 0;
    posix_shm_cb->ipc_cb.error_code = 0;
    return true;
}

bool posix_shm_send_msg(POSIX_SHM_CB *posix_shm_cb, POSIX_SHM_MSG *posix_shm_msg)
{
    if(posix_shm_cb->ipc_cb.success != 666)
    {
        posix_shm_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(posix_shm_cb->ipc_cb.type != IPC_MSG_SEND)
    {
        posix_shm_cb->ipc_cb.error_code = -3; // ipc_cb通信类型不相符
        return false;
    }

    int times = 5; //尝试次数
    bool b = false;
    int t1,t2;
    while(times--)
    {
        int t1 = sem_trywait(posix_shm_cb->mutex_wr);
        if(t1 < 0)
        {
            if(errno == EAGAIN)
            {
                continue;
            }else{
                posix_shm_cb->ipc_cb.error_code = -14; //发送报错
                //printf("[errno]:%d\n", errno);
                return false;
            }
        }else{
            posix_shm_cb->ptr->len = posix_shm_msg->len;
            memcpy(posix_shm_cb->ptr->msg, posix_shm_msg->msg, posix_shm_msg->len);
            //printf("[TEST]：　%s",posix_shm_msg->msg);
            posix_shm_cb->ptr->msg[posix_shm_msg->len] = '\0';
            t2 =  sem_post(posix_shm_cb->mutex_rd);
            if(t2 < 0)
            {
                posix_shm_cb->ipc_cb.error_code = -15; //发送报错
                //printf("[errno]:%d\n", errno);
                return false;
            }
            b = true;
            break;
        }
    }
    if(!b)
    {
        posix_shm_cb->ipc_cb.error_code = 10; //消息没有被读取
        return false;
    }

    posix_shm_cb->ipc_cb.error_code = 0;
    return true;
}

int posix_shm_get_errcode(POSIX_SHM_CB *posix_shm_cb)
{
    return posix_shm_cb->ipc_cb.error_code;
}

bool posix_shm_receive_msg(POSIX_SHM_CB *posix_shm_cb, POSIX_SHM_MSG *posix_shm_msg)
{
    if(posix_shm_cb->ipc_cb.success != 666)
    {
        posix_shm_cb->ipc_cb.error_code = -666; // ipc_cb尚未初始化, 就被调用
        return false;
    }
    if(posix_shm_cb->ipc_cb.type != IPC_MSG_RECEIVE)
    {
        posix_shm_cb->ipc_cb.error_code = -5; // ipc_cb通信类型不相符
        return false;
    }

    int t1 = sem_wait(posix_shm_cb->mutex_rd);
    if(t1 < 0)
    {
        posix_shm_cb->ipc_cb.error_code = -16;
        //printf("[errno]:%d\n", errno);
        return false;
    }

    posix_shm_msg->len = posix_shm_cb->ptr->len;
    memcpy(posix_shm_msg->msg, posix_shm_cb->ptr->msg, posix_shm_msg->len);
    posix_shm_msg->msg[posix_shm_msg->len] = '\0';
    t1 = sem_post(posix_shm_cb->mutex_wr);
    if(t1 < 0)
    {
        posix_shm_cb->ipc_cb.error_code = -17;
        //printf("[errno]:%d\n", errno);
        return false;
    }

//    //printf("接受信息字节数: %d\n", t);
//    //printf("%d %s\n", posix_shm_msg->len, posix_shm_msg->msg);

    posix_shm_cb->ipc_cb.error_code = 0;
    return true;
}


