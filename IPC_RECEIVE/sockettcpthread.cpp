#include "sockettcpthread.h"
#include <QDebug>

SocketTCPThread::SocketTCPThread(QString receive_ip, ushort receive_port)
{
    this->state = 0;
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
    if(this->state == 0)
        return;
    bool b;
    SOCKET_TCP_CB *socket_tcp_cb = (SOCKET_TCP_CB *)this->ipc_cb;

    char ip_2[PATH_MAX];
    QString2Char(this->receive_ip, ip_2);
    qDebug() << "开始连接...";
    b = init_socket_tcp_cb(socket_tcp_cb, IPC_MSG_RECEIVE, "127.0.0.1", 9,0, ip_2, strlen(ip_2), this->receive_port);
    if(!b){
        emit signal_msg(common_msg('B', socket_tcp_get_errcode(socket_tcp_cb), errno, "初始化失败"));
        qDebug() << "初始化失败...";
        return;
    }
    qDebug() << "socket here";
    b = socket_tcp_accept(socket_tcp_cb);
    if(!b){
        emit signal_msg(common_msg('B', socket_tcp_get_errcode(socket_tcp_cb), errno, "监听失败"));
        qDebug() << "监听失败...";
        return;
    }else{
        emit signal_msg(common_msg('A', socket_tcp_get_errcode(socket_tcp_cb), errno, "连接发送端成功"));
        qDebug() << "连接成功...";
    }
    this->state = 2;

    SOCKET_TCP_MSG socket_tcp_msg;


    while(true)
    {
        qDebug() << "开始接收...";
        b = socket_tcp_receive_msg(socket_tcp_cb, &socket_tcp_msg);
        if(!b){
            qDebug() << "接收失败";
            int err_code = socket_tcp_get_errcode(socket_tcp_cb);
            if(err_code != 0)
                emit signal_msg(common_msg('D',err_code , errno, "接收消息失败"));
            else
                emit signal_msg(common_msg('D',err_code , errno, "发送方关闭连接"));
            break;
        }else{
            qDebug() << "接收成功";
            emit signal_msg(common_msg('C', socket_tcp_get_errcode(socket_tcp_cb), errno,QString(socket_tcp_msg.msg)));
        }
    }


}
