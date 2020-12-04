#include "receive_thread.h"
#include <QDebug>
ReceiveThread::ReceiveThread()
{
    this->success = false;
}

ReceiveThread::~ReceiveThread()
{
    qDebug() << "线程关闭";
    qDebug() << this->ipc_cb->method;
    if(this->ipc_cb->method == IPC_SYSTEMV_SHM)
    {
        close_sysv_shm_cb((SYSV_SHM_CB*)this->ipc_cb);
        delete (SYSV_SHM_CB*)this->ipc_cb;
    }
    else if(this->ipc_cb->method == IPC_POSIX_SHM)
    {
        close_posix_shm_cb((POSIX_SHM_CB*)this->ipc_cb);
        delete (POSIX_SHM_CB*)this->ipc_cb;
    }
}


void ReceiveThread::run()
{
    if(this->success == false) return;
    switch (this->ipc_cb->method) {
        case IPC_SYSTEMV_MQUEUE:
        {
            qDebug() << "SYSV - thread";
            SYSV_MQ_CB * sysv_mq_cb = (SYSV_MQ_CB *)this->ipc_cb;
            SYSV_MQ_MSG sysv_mq_msg;
            bool b;
            while(true)
            {
                b = sysv_mq_receive_msg(sysv_mq_cb, &sysv_mq_msg); // block thread
                //qDebug()<<errno;

                if(!b) {
                    common_msg cmsg;
                    cmsg.tmp1 = sysv_mq_get_errcode(sysv_mq_cb);
                    cmsg.msg = QString("SYSVMQ接收消息出错，错误码：%1").arg(cmsg.tmp1);
                    emit signal_msg(this->ipc_cb->method, cmsg, 0);
                   //emit signal_msg(this->ipc_cb->method, QString("%1").arg(sysv_mq_get_errcode(sysv_mq_cb)), 0);
                   //printf("[error]:接收消息过程中出错x %d\n", sysv_mq_get_errcode(sysv_mq_cb));
                   break;
                }else{
                    common_msg cmsg;
                    cmsg.tmp1 = sysv_mq_msg.len;
                    cmsg.tmp2 = sysv_mq_msg.mtype;
                    cmsg.msg = QString(sysv_mq_msg.msg);
                    emit signal_msg(this->ipc_cb->method, cmsg , 1);
                    //emit signal_msg(this->ipc_cb->method, QString(sysv_mq_msg.msg), 1);
                }
                //printf("接收到了消息: %s\n", sysv_mq_msg.msg);
                //qDebug() << "开始监听...";
            }
            break;
        }
        case IPC_POSIX_MQUEUE:
        {
            qDebug() << "POSIX - thread";
            POSIX_MQ_CB * posix_mq_cb = (POSIX_MQ_CB *)this->ipc_cb;
            POSIX_MQ_MSG posix_mq_msg;
            bool b;
            while(true)
            {
                b = posix_mq_receive_msg(posix_mq_cb, &posix_mq_msg); // block thread
                //qDebug()<<errno;

                if(!b) {
                    common_msg cmsg;
                    cmsg.tmp1 = posix_mq_get_errcode(posix_mq_cb);
                    cmsg.msg = QString("POSIXMQ接收消息出错，错误码：%1").arg(cmsg.tmp1);
                   //emit signal_msg(this->ipc_cb->method, QString("%1").arg(posix_mq_get_errcode(posix_mq_cb)), 0);
                     emit signal_msg(this->ipc_cb->method, cmsg, 0);
                   //printf("[error]:接收消息过程中出错x %d\n", sysv_mq_get_errcode(sysv_mq_cb));
                   break;
                }else{
                    common_msg cmsg;
                    cmsg.tmp1 = posix_mq_msg.len;
                    cmsg.tmp2 = posix_mq_msg.mtype;
                    cmsg.msg = QString(posix_mq_msg.msg);
                    emit signal_msg(this->ipc_cb->method, cmsg , 1);
                }
            }
            break;
        }
        case IPC_FIFO:
        {
            qDebug() << "FIFO - thread";
            FIFO_CB * fifo_cb = (FIFO_CB *)this->ipc_cb;
            FIFO_MSG fifo_msg;
            bool b;
            while(true)
            {
                b = fifo_receive_msg(fifo_cb, &fifo_msg); // block thread
                //qDebug()<<errno;

                if(!b) {
                    common_msg cmsg;
                    cmsg.tmp1 = fifo_get_errcode(fifo_cb);
                    cmsg.msg = QString("FIFO接收消息出错，错误码：%1").arg(cmsg.tmp1);
                    emit signal_msg(this->ipc_cb->method, cmsg, 0);
                   //emit signal_msg(this->ipc_cb->method, QString("%1").arg(fifo_get_errcode(fifo_cb)), 0);
                   break;
                }else{
                    common_msg cmsg;
                    cmsg.tmp1 = fifo_msg.len;
                    cmsg.msg = QString(fifo_msg.msg);
                    emit signal_msg(this->ipc_cb->method, cmsg , 1);
                    //emit signal_msg(this->ipc_cb->method, QString(fifo_msg.msg), 1);
                }
            }
            break;
        }
        case IPC_POSIX_SHM:
        {
            qDebug() << "POSIXSHM - thread";
            POSIX_SHM_CB * posix_shm_cb = (POSIX_SHM_CB *)this->ipc_cb;
            POSIX_SHM_MSG posix_shm_msg;
            bool b;
            while(true)
            {
                b = posix_shm_receive_msg(posix_shm_cb, &posix_shm_msg); // block thread
                //qDebug()<<errno;

                if(!b) {
                    common_msg cmsg;
                    cmsg.tmp1 = posix_shm_get_errcode(posix_shm_cb);
                    cmsg.msg = QString("POSIXSHM接收消息出错，错误码：%1").arg(cmsg.tmp1);
                    emit signal_msg(this->ipc_cb->method, cmsg , 0);
                   //emit signal_msg(this->ipc_cb->method, QString("%1").arg(posix_shm_get_errcode(posix_shm_cb)), 0);
                   break;
                }else{
                    common_msg cmsg;
                    cmsg.tmp1 = posix_shm_msg.len;
                    cmsg.msg = QString(posix_shm_msg.msg);
                    emit signal_msg(this->ipc_cb->method, cmsg , 1);
                    //emit signal_msg(this->ipc_cb->method, QString(posix_shm_msg.msg), 1);
                }
            }
            break;
        }
        case IPC_SYSTEMV_SHM:
        {
            qDebug() << "SYSVSHM - thread";
            SYSV_SHM_CB* sysv_shm_cb = (SYSV_SHM_CB *)this->ipc_cb;

            SYSV_SHM_MSG sysv_shm_msg;
            bool b;
            while(true)
            {
                b = sysv_shm_receive_msg(sysv_shm_cb, &sysv_shm_msg); // block thread
                //qDebug()<<errno;

                if(!b) {
                    common_msg cmsg;
                    cmsg.tmp1 = sysv_shm_get_errcode(sysv_shm_cb);
                    cmsg.msg = QString("SYSVSHM接收消息出错，错误码：%1").arg(cmsg.tmp1);
                    emit signal_msg(this->ipc_cb->method, cmsg , 0);
                   //emit signal_msg(this->ipc_cb->method, QString("%1").arg(sysv_shm_get_errcode(sysv_shm_cb)), 0);
                   //printf("[error]:接收消息过程中出错x %d\n", sysv_shm_get_errcode(sysv_shm_cb));
                   break;
                }else{
                    common_msg cmsg;
                    cmsg.tmp1 = sysv_shm_msg.len;
                    cmsg.msg = QString(sysv_shm_msg.msg);
                    emit signal_msg(this->ipc_cb->method, cmsg , 1);
                   // emit signal_msg(this->ipc_cb->method, QString(sysv_shm_msg.msg), 1);
                }
            }
            break;
        }
        case IPC_SOCKET_UDP:
        {
            qDebug() << "SOCKET_UDP - thread";
            SOCKET_UDP_CB * socket_udp_cb = (SOCKET_UDP_CB *)this->ipc_cb;
            SOCKET_UDP_MSG socket_udp_msg;
            bool b;
            while(true)
            {
                b = socket_udp_receive_msg(socket_udp_cb, &socket_udp_msg); // block thread

                if(!b) {
                    common_msg cmsg;
                    cmsg.tmp1 = socket_udp_get_errcode(socket_udp_cb);
                    cmsg.msg = QString("SYSVSHM接收消息出错，错误码：%1").arg(cmsg.tmp1);
                    emit signal_msg(this->ipc_cb->method, cmsg , 0);
                   //emit signal_msg(this->ipc_cb->method, QString("%1").arg(socket_udp_get_errcode(socket_udp_cb)), 0);
                   //printf("[error]:接收消息过程中出错x %d\n", SOCKET_UDP_get_errcode(SOCKET_UDP_cb));
                   break;
                }else{
                    common_msg cmsg;
                    cmsg.tmp1 = socket_udp_msg.len;
                    cmsg.msg = QString(socket_udp_msg.msg);
                    emit signal_msg(this->ipc_cb->method, cmsg , 1);
                    //emit signal_msg(this->ipc_cb->method, QString(socket_udp_msg.msg), 1);
                }
            }
            break;
        }
        default:
        {

        }
    }
    this->success = false;
    this->quit();
}


void ReceiveThread::set_ipc_cb(IPC_CB *ipc_cb)
{
    this->ipc_cb = ipc_cb;
    this->success = true;
}

void ReceiveThread::slot_close_thread()
{
    this->quit();
    qDebug() << "线程关闭执行";
}
