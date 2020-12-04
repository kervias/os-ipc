#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QThread>
#include <QCloseEvent>
#include <QtCore>
#include <QMap>


#include "common/model.h"
#include "receive_thread.h"
#include "posixmq/posixmq.h"
#include "fifo/cus_fifo.h"
#include "posixshm/posixshm.h"
#include "sysvshm/sysvshm.h"
#include "socket_udp/socket_udp.h"
#include "socket_tcp/socket_tcp.h"
#include "uicommon.h"
#include "sockettcpthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


#define IPC_NUM 7

struct IPC_CTX{
   bool success; //　当前是否处于激活状态
   ReceiveThread *thread; //对应的接收线程
   IPC_CB *ipc_cb; //　对应的IPC_CB
   IPC_CTX():success(false), thread(nullptr), ipc_cb(nullptr){}
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initMainWindow(); //初始化窗口样式
    void initEventBind(); //初始化事件绑定

    // sysv_mq相关
    bool sysvmq_start_connect();
    bool sysvmq_stop_connect();

    // posix_mq相关
    bool posixmq_start_connect();
    bool posixmq_stop_connect();

    // fifo相关
    bool fifo_start_connect();
    bool fifo_stop_connect();

    // posix_shm相关
    bool posixshm_start_connect();
    bool posixshm_stop_connect();

    // sysv_shm相关
    bool sysvshm_start_connect();
    bool sysvshm_stop_connect();

    bool socketudp_start_connect();
    bool socketudp_stop_connect();

    bool sockettcp_start_connect();
    bool sockettcp_stop_connect();


    // 辅助函数
    char* QStringToChar(QString str); //QString 转换为char*
    void QString2Char(QString src, char *dest);

    // 重写QT虚函数
    void closeEvent(QCloseEvent *event);

public slots:
    void slot_currentItemChanged(); //QListItem改变, stackWidget改变

    // sysv_mq相关
    void slot_sysvmq_btn_enter(); //确认连接并初始化或取消连接
    void slot_posixmq_btn_enter();
    void slot_fifo_btn_enter();
    void slot_posixshm_btn_enter();
    void slot_sysvshm_btn_enter();
    void slot_socketudp_btn_enter();
    void slot_sockettcp_btn_enter();

    // common函数
    void slot_common_receive_msg(IPC_METHOD ipc_method, common_msg cmsg, bool type); //通用消息接收槽函数

    void slot_sockettcp_receive_msg(common_msg cmsg);

signals:
    void signal_sysv_mq_stop_thread();
    void signal_posix_mq_stop_thread();
    void signal_fifo_stop_thread();
    void signal_posix_shm_stop_thread();
    void signal_sysv_shm_stop_thread();
    void signal_socket_udp_stop_thread();
    void signal_socket_tcp_stop_thread();

private:
    Ui::MainWindow *ui;
    IPC_CTX ipc_ctx[IPC_NUM]; // 7个IPC
    QMap<IPC_METHOD, int> mapIPC; //每个IPC对应的编号

    int socket_tcp_state;
    SocketTCPThread *socket_tcp_thread;
};
#endif // MAINWINDOW_H
