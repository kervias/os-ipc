#ifndef SYSV_MQ_THREAD_H
#define SYSV_MQ_THREAD_H

#include <QThread>
#include <QDebug>
#include "sysvmq/sysvmq.h"
#include "posixmq/posixmq.h"
#include "fifo/cus_fifo.h"
#include "posixshm/posixshm.h"
#include "sysvshm/sysvshm.h"
#include "socket_udp/socket_udp.h"
#include "uicommon.h"

class ReceiveThread : public QThread
{
    Q_OBJECT
public:
    ReceiveThread();
    ~ReceiveThread();
    void set_ipc_cb(IPC_CB *ipc_cb);

signals:
    void signal_msg(IPC_METHOD ipc_method, common_msg cmsg, bool type); //接收到消息，通过信号发送消息
    //void signal_error(IPC_METHOD ipc_method, QString msg); //接受函数出错，发送给主线程

public slots:
    void slot_close_thread();

protected:
    void run();

private:
    IPC_CB *ipc_cb;
    bool success;
};



#endif // SYSV_MQ_THREAD_H
