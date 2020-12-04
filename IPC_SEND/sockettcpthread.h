#ifndef SOCKETTCPTHREAD_H
#define SOCKETTCPTHREAD_H

#include <QObject>
#include <QThread>
#include "socket_tcp/socket_tcp.h"
#include "uicommon.h"

class SocketTCPThread : public QThread
{
    Q_OBJECT
public:
    SocketTCPThread(QString send_ip, ushort send_port, QString receive_ip, ushort receive_port);
    void QString2Char(QString src, char *dest);
    void set_ipc_cb(IPC_CB *ipc_cb);

protected:
    void run();

signals:
    void signal_msg(common_msg cmsg);

public slots:
    void slot_send_msg(common_msg cmsg);

private:
    IPC_CB *ipc_cb;
    int state;
    // state: 0 未设置ipc_cb
    // state: 1 已设置ipc_cb,未connect
    // state: 2 已经connect
    QString send_ip;
    ushort send_port;
    QString receive_ip;
    ushort receive_port;
    common_msg cmsg;
};

#endif // SOCKETTCPTHREAD_H
