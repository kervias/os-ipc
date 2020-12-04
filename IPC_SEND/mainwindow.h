#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QCloseEvent>
#include "common/model.h"
#include "posixmq/posixmq.h"
#include "fifo/cus_fifo.h"
#include "posixshm/posixshm.h"
#include "sysvshm/sysvshm.h"
#include "socket_udp/socket_udp.h"
#include "socket_tcp/socket_tcp.h"
#include "sockettcpthread.h"
#include "uicommon.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


#define IPC_NUM 7

struct IPC_CTX{
   bool success; //　当前IPC是否处于激活状态
   IPC_CB *ipc_cb; //　对应的IPC_CB
   IPC_CTX():success(false), ipc_cb(nullptr){}
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initMainWindow(); //初始化窗口样式
    void initEventBind(); //初始化事件绑定

    // 辅助函数
    char* QStringToChar(QString str); //QString 转换为char*
    void QString2Char(QString src, char *dest);


    bool sysvmq_start_connect();
    bool sysvmq_stop_connect();

    bool posixmq_start_connect();
    bool posixmq_stop_connect();

    bool fifo_start_connect();
    bool fifo_stop_connect();

    bool posixshm_start_connect();
    bool posixshm_stop_connect();

    bool sysvshm_start_connect();
    bool sysvshm_stop_connect();

    bool socketudp_start_connect();
    bool socketudp_stop_connect();

    bool sockettcp_start_connect();
    bool sockettcp_stop_connect();

    void closeEvent(QCloseEvent *);

public slots:
    void slot_currentItemChanged(); //QListItem改变, stackWidget改变

    void slot_sysvmq_btn_enter(); //开启按钮或关闭按钮
    void slot_sysvmq_send_msg(); //发送按钮

    void slot_posixmq_btn_enter();
    void slot_posixmq_send_msg(); //发送按钮

    void slot_fifo_btn_enter();
    void slot_fifo_send_msg();

    void slot_posixshm_btn_enter();
    void slot_posixshm_send_msg();

    void slot_sysvshm_btn_enter();
    void slot_sysvshm_send_msg();

    void slot_socketudp_btn_enter();
    void slot_socketudp_send_msg();

    void slot_sockettcp_btn_enter();
    void slot_sockettcp_send_msg();
    void slot_sockettcp_tip_msg(common_msg cmsg);

signals:
    void signal_sockettcp_send_msg(common_msg cmsg);

private:
    Ui::MainWindow *ui;

    IPC_CTX ipc_ctx[IPC_NUM];  //7个IPC
    QMap<IPC_METHOD, int> mapIPC; //每个IPC对应的编号

    int socket_tcp_state;
    SocketTCPThread *socket_tcp_thread;
};
#endif // MAINWINDOW_H
