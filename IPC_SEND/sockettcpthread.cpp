#include "sockettcpthread.h"
#include <QDebug>
SocketTCPThread::SocketTCPThread(QString send_ip, ushort send_port, QString receive_ip, ushort receive_port)
{
    this->state = 0;
    this->send_ip = send_ip;
    this->send_port = send_port;
    this->receive_ip = receive_ip;
    this->receive_port = receive_port;
}

void SocketTCPThread::QString2Char(QString src, char* dest)
{
    char *t = src.toUtf8().data();
    memcpy(dest, t, strlen(t));
    dest[strlen(t)] = '\0';
}

void SocketTCPThread::set_ipc_cb(IPC_CB *ipc_cb)
{
    this->ipc_cb = ipc_cb;
    this->state = 1;
}

void SocketTCPThread::run()
{
    if(this->state == 0) return;
    bool b;
    SOCKET_TCP_CB *socket_tcp_cb = (SOCKET_TCP_CB *)this->ipc_cb;
    if(this->state == 1)
    {
        char ip_1[PATH_MAX];
        char ip_2[PATH_MAX];
        QString2Char(this->send_ip, ip_1);
        QString2Char(this->receive_ip, ip_2);
        qDebug() << "开始连接...";
        b = init_socket_tcp_cb(socket_tcp_cb, IPC_MSG_SEND, ip_1, strlen(ip_1), this->send_port, ip_2, strlen(ip_2), this->receive_port);
        if(!b){
            //printf("[error]:send初始化过程中出错 %d\n", socket_tcp_get_errcode(&socket_tcp_cb));
            emit signal_msg(common_msg('B', socket_tcp_get_errcode(socket_tcp_cb), errno, "连接接收端失败"));
            qDebug() << "连接失败...";
            return;
        }else{
            emit signal_msg(common_msg('A', socket_tcp_get_errcode(socket_tcp_cb), errno, "连接接收端成功"));
            qDebug() << "连接成功...";
        }
        this->state = 2;
    }
    else if(this->state == 2)
    {
         SOCKET_TCP_MSG socket_tcp_msg;
         QString2Char(this->cmsg.msg, socket_tcp_msg.msg);
         socket_tcp_msg.len = strlen(socket_tcp_msg.msg);
         qDebug() << "开始发送...";
         b = socket_tcp_send_msg(socket_tcp_cb, &socket_tcp_msg);
         if(!b){
             qDebug() << "发送失败";
              //printf("[error]:sendmsg中出错 %d\n", socket_tcp_get_errcode(socket_tcp_cb));
           emit signal_msg(common_msg('D', socket_tcp_get_errcode(socket_tcp_cb), errno, "发送消息失败"));
         }else{
             qDebug() << "发送成功";
           emit signal_msg(common_msg('C', socket_tcp_get_errcode(socket_tcp_cb), errno, "发送消息成功"));
         }
    }
}

void SocketTCPThread::slot_send_msg(common_msg cmsg)
{
    this->cmsg.msg = cmsg.msg;
    this->cmsg.tmp1 = cmsg.tmp1;
    this->cmsg.tmp2 = cmsg.tmp2;
    this->cmsg.type = cmsg.type;
}


