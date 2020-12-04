#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QString>
#include <QSizePolicy>
#include <QDebug>
#include <QMessageBox>
#include "sysvmq/sysvmq.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<IPC_METHOD>("IPC_METHOD");
    qRegisterMetaType<common_msg>("common_msg");
    socket_tcp_state = 0;
    socket_tcp_thread = nullptr;
    mapIPC[IPC_FIFO] = 0;
    mapIPC[IPC_POSIX_MQUEUE] = 1;
    mapIPC[IPC_POSIX_SHM] = 2;
    mapIPC[IPC_SYSTEMV_MQUEUE] = 3;
    mapIPC[IPC_SYSTEMV_SHM] = 4;
    mapIPC[IPC_SOCKET_UDP] = 5;
    mapIPC[IPC_SOCKET_TCP] = 6;

    this->initMainWindow();
    this->initEventBind();
}

void MainWindow::initEventBind()
{
    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &MainWindow::slot_currentItemChanged);

    connect(ui->fifo_btn_enter,&QPushButton::released, this, &MainWindow::slot_fifo_btn_enter);

    connect(ui->sysvmq_btn_enter, &QPushButton::released, this, &MainWindow::slot_sysvmq_btn_enter);

    connect(ui->posixmq_btn_enter, &QPushButton::released, this, &MainWindow::slot_posixmq_btn_enter);

    connect(ui->posixshm_btn_enter, &QPushButton::released, this, &MainWindow::slot_posixshm_btn_enter);

    connect(ui->sysvshm_btn_enter, &QPushButton::released, this, &MainWindow::slot_sysvshm_btn_enter);

    connect(ui->socketudp_btn_enter, &QPushButton::released, this, &MainWindow::slot_socketudp_btn_enter);

    connect(ui->sockettcp_btn_enter, &QPushButton::released, this, &MainWindow::slot_sockettcp_btn_enter);

}


// 初始化窗口
void MainWindow::initMainWindow()
{

    this->setWindowTitle("接收进程");
    ui->splitter->setStretchFactor(0,35);
    ui->splitter->setStretchFactor(1,65);


    pid_t pid = getpid();
    ui->statusbar->showMessage("当前进程ID: "+QString::number(pid));
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);

}


void MainWindow::slot_currentItemChanged()
{
    int item_index = ui->listWidget->currentRow();
    if(ui->stackedWidget->count() >= item_index+1)
        ui->stackedWidget->setCurrentIndex(item_index);

    else
        qDebug()<< "页的数目过少";
}

void MainWindow::slot_fifo_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_FIFO]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->fifo_start_connect();
        if(b) QMessageBox::information(this, "INFO", "FIFO 开启成功！", QMessageBox::Ok);
    }else{
        b = this->fifo_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "FIFO关闭成功！", QMessageBox::Ok);
    }
}


void MainWindow::slot_sysvmq_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_SYSTEMV_MQUEUE]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->sysvmq_start_connect();
        if(b) QMessageBox::information(this, "INFO", "System V MQ开启成功！", QMessageBox::Ok);
    }else{
        b = this->sysvmq_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "System V MQ关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_posixmq_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_POSIX_MQUEUE]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->posixmq_start_connect();
        if(b) QMessageBox::information(this, "INFO", "POSIX MQ开启成功！", QMessageBox::Ok);
    }else{
        b = this->posixmq_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "POSIX MQ关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_posixshm_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_POSIX_SHM]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->posixshm_start_connect();
        if(b) QMessageBox::information(this, "INFO", "POSIX SHM开启成功！", QMessageBox::Ok);
    }else{
        b = this->posixshm_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "POSIX SHM关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_socketudp_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_SOCKET_UDP]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->socketudp_start_connect();
        if(b) QMessageBox::information(this, "INFO", "IPC_SOCKET_UDP开启成功！", QMessageBox::Ok);
    }else{
        b = this->socketudp_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "IPC_SOCKET_UDP关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_sockettcp_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_SOCKET_TCP]].success) //已经初始化了，再次点击将取消连接
    {
//        if(this->socket_tcp_state == 1)
//        {
//            QMessageBox::information(this, "INFO", "SOCKET TCP正在连接中！", QMessageBox::Ok);
//            return;
//        }
        b = this->sockettcp_start_connect();
//        if(b) QMessageBox::information(this, "INFO", "SOCKET UDP开启成功！", QMessageBox::Ok);
    }else{
        if(this->socket_tcp_state == 1)
        {
            b = this->sockettcp_stop_connect();
            if(b) QMessageBox::information(this, "INFO", "SOCKET TCP关闭成功！", QMessageBox::Ok);
        }else{
            QMessageBox::information(this, "INFO", "主动关闭接收端已禁用，需要发送端主动关闭！", QMessageBox::Ok);
        }

    }
}

bool MainWindow::sockettcp_start_connect()
{
    ui->sockettcp_textBrowser->clear();
    ui->sockettcp_textBrowser->clearHistory();
    SOCKET_TCP_CB *socket_tcp_cb = new SOCKET_TCP_CB;
    int ind = mapIPC[IPC_SOCKET_TCP];
    // 输入参数获取并检查
//    QString qpath1 = ui->sockettcp_lineEdit01->text();
//    if(qpath1.length() == 0)
//    {
//        QMessageBox::warning(this, "ERROR", "sockettcp 发送端ip地址不能为空", QMessageBox::Ok);
//        return false;
//    }
//    QString qpath2 = ui->sockettcp_lineEdit02->text();
//    if(qpath2.length() == 0)
//    {
//        QMessageBox::warning(this, "ERROR", "sockettcp 发送端端口不能为空", QMessageBox::Ok);
//        return false;
//    }
    QString qpath3 = ui->sockettcp_lineEdit03->text();
    if(qpath3.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "sockettcp server端ip地址不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath4 = ui->sockettcp_lineEdit04->text();
    if(qpath4.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "sockettcp server端端口不能为空", QMessageBox::Ok);
        return false;
    }

//    char path1[PATH_MAX];
//    ushort port1 = qpath2.toUShort();
    char path3[PATH_MAX];
    ushort port2 = qpath4.toUShort();
//    QString2Char(qpath1, path1);
    QString2Char(qpath3, path3);

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)socket_tcp_cb;
    ui->sockettcp_state_label->setText("状态：正在等待发送方连接...");
    this->socket_tcp_state = 1;
    this->socket_tcp_thread = new SocketTCPThread(qpath3,port2);
    this->socket_tcp_thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_socket_tcp_stop_thread, this->socket_tcp_thread, &SocketTCPThread::quit);
    connect(this->socket_tcp_thread, &SocketTCPThread::finished, this->socket_tcp_thread, &SocketTCPThread::deleteLater);
    connect(this->socket_tcp_thread, &SocketTCPThread::signal_msg, this, &MainWindow::slot_sockettcp_receive_msg);
    this->socket_tcp_thread->start();
    this->ipc_ctx[ind].success = true;
    ui->sockettcp_btn_enter->setText("关闭");
    return true;
}
bool MainWindow::sockettcp_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SOCKET_TCP];
    SOCKET_TCP_CB *socket_tcp_cb = (SOCKET_TCP_CB *)this->ipc_ctx[ind].ipc_cb;
    //qDebug() << "你好1";
    if(this->socket_tcp_state == 1 || this->socket_tcp_state == 3)
    {
        //qDebug() << "hekke";
        emit signal_socket_tcp_stop_thread();
    }
    //qDebug() << "你好2";

    //if(this->socket_tcp_thread->isRunning()) this->socket_tcp_thread->terminate();
    ui->sockettcp_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    //socket_tcp_cb->ipc_cb.success = 666;
    //qDebug() << "你好3";
    b = close_socket_tcp_cb(socket_tcp_cb);
   // qDebug() << (this->socket_tcp_thread == nullptr);
    this->socket_tcp_state = 0;
    //qDebug() << "你好4";
    //disconnect(this,&MainWindow::signal_socket_tcp_stop_thread, this->socket_tcp_thread, &SocketTCPThread::quit);
    //disconnect(this->socket_tcp_thread, &SocketTCPThread::signal_msg, this, &MainWindow::slot_sockettcp_receive_msg);
    //delete this->socket_tcp_thread;
    // qDebug() << "你好5";
    this->socket_tcp_thread = nullptr;
    ui->sockettcp_btn_enter->setText("开启");
    ui->sockettcp_state_label->setText("状态：未连接");
//    if(!b){
//        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(socket_tcp_get_errcode(socket_tcp_cb)), QMessageBox::Ok);
//        return false;
//    }
    delete socket_tcp_cb;
    return true;
}
void MainWindow::slot_sockettcp_receive_msg(common_msg cmsg)
{
    if(cmsg.type == 'A') //连接成功
    {
        this->socket_tcp_state = 3;
        ui->sockettcp_state_label->setText("状态：已连接");
    }else if(cmsg.type == 'B')//连接失败
    {
        this->socket_tcp_state = 0;
        QMessageBox::warning(this, "ERROR", QString("%1 \n错误码: %2\n errno:%3").arg(cmsg.msg).arg(cmsg.tmp1).arg(cmsg.tmp2), QMessageBox::Ok);
        this->sockettcp_stop_connect();
    }else if(cmsg.type == 'C')//接收消息成功
    {
        QString msg=cmsg.msg;
        QString qmsg;
        msg = msg.replace("<","&lt;"); //#6c757d
        msg = msg.replace(">","&gt;");
        msg = msg.replace(" ","&nbsp;");
        msg = msg.replace("\n","<br>");
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
        qmsg = QString("<div>"
                        "<small>&nbsp;</small>"
                        "<p align=left style=\"font-weight: bold; color: orange;\">[SOCKET_TCP_MSG]:</p>"
                        "<p align=left style='color: black'>%1</p>"
                        "<div align=right style=\"color:blue;\">%2</div>"
                        "</div>"
                       ).arg(msg).arg(current_date);
        ui->sockettcp_textBrowser->append(qmsg);
    }else if(cmsg.type == 'D')//接收消息失败
    {
        if(cmsg.tmp1 == 0)
        {
            QMessageBox::information(this, "INFO","发送方主动关闭了连接",QMessageBox::Ok);
            this->sockettcp_stop_connect();
        }else{
            QMessageBox::warning(this, "警告",QString("SOCKET_TCP接收失败\n%1 \n错误码: %2\n errno:%3").arg(cmsg.msg).arg(cmsg.tmp1).arg(cmsg.tmp2), QMessageBox::Ok);
            this->sockettcp_stop_connect();
        }
    }
}

void MainWindow::slot_sysvshm_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_SYSTEMV_SHM]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->sysvshm_start_connect();
        if(b) QMessageBox::information(this, "INFO", "SYSV SHM开启成功！", QMessageBox::Ok);
    }else{
        b = this->sysvshm_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "SYSV SHM关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_common_receive_msg(IPC_METHOD ipc_method, common_msg cmsg, bool type)
{
    QString qmsg;
    QString msg = cmsg.msg;
    if(ipc_method == IPC_SYSTEMV_MQUEUE)
    {
        if(type)
        {
            msg = msg.replace("<","&lt;"); //#6c757d
            msg = msg.replace(">","&gt;");
            msg = msg.replace(" ","&nbsp;");
            msg = msg.replace("\n","<br>");
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            qmsg = QString("<div>"
                            "<small>&nbsp;</small>"
                            "<p align=left style=\"font-weight: bold; color: orange;\">[SYSV_MQ_MSG]: - 优先级[%2]</p>"
                            "<p align=left style='color: black'>%1</p>"
                            "<div align=right style=\"color:blue;\">%3</div>"
                            "</div>"
                           ).arg(msg).arg(cmsg.tmp2).arg(current_date);
            ui->sysvmq_textBrowser->append(qmsg);
        }else
        {
            QMessageBox::warning(this, "ERROR", QString("SYSV_MQ接收消息出错．错误码: %1．将关闭该IPC").arg(msg), QMessageBox::Ok);
            this->sysvmq_stop_connect();
        }
    }else if(ipc_method == IPC_POSIX_MQUEUE)
    {
        if(type)
        {
            msg = msg.replace("<","&lt;"); //#6c757d
            msg = msg.replace(">","&gt;");
            msg = msg.replace(" ","&nbsp;");
            msg = msg.replace("\n","<br>");
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            qmsg = QString("<div>"
                            "<small>&nbsp;</small>"
                            "<p align=left style=\"font-weight: bold; color: orange;\">[POSIX_MQ_MSG]: - 优先级[%2]</p>"
                            "<p align=left style='color: black'>%1</p>"
                            "<div align=right style=\"color:blue;\">%3</div>"
                            "</div>"
                           ).arg(msg).arg(cmsg.tmp2).arg(current_date);
            ui->posixmq_textBrowser->append(qmsg);
        }else
        {
            QMessageBox::warning(this, "ERROR", QString("POSIX_MQ接收消息出错．错误码: %1．将关闭该IPC").arg(msg), QMessageBox::Ok);
            this->posixmq_stop_connect();
        }
    }else if(ipc_method == IPC_FIFO)
    {
        if(type)
        {
            msg = msg.replace("<","&lt;"); //#6c757d
            msg = msg.replace(">","&gt;");
            msg = msg.replace(" ","&nbsp;");
            msg = msg.replace("\n","<br>");
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            qmsg = QString("<div>"
                            "<small>&nbsp;</small>"
                            "<p align=left style=\"font-weight: bold; color: orange;\">[FIFO_MSG]:</p>"
                            "<p align=left style='color: black'>%1</p>"
                            "<div align=right style=\"color:blue;\">%2</div>"
                            "</div>"
                           ).arg(msg).arg(current_date);
            ui->fifo_textBrowser->append(qmsg);
        }else
        {
            QMessageBox::warning(this, "ERROR", QString("FIFO接收消息出错．错误码: %1．将关闭该IPC").arg(msg), QMessageBox::Ok);
            this->fifo_stop_connect();
        }
    }else if(ipc_method == IPC_POSIX_SHM)
    {
        if(type)
        {
            msg = msg.replace("<","&lt;"); //#6c757d
            msg = msg.replace(">","&gt;");
            msg = msg.replace(" ","&nbsp;");
            msg = msg.replace("\n","<br>");
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            qmsg = QString("<div>"
                            "<small>&nbsp;</small>"
                            "<p align=left style=\"font-weight: bold; color: orange;\">[POSIXSHM_MSG]:</p>"
                            "<p align=left style='color: black'>%1</p>"
                            "<div align=right style=\"color:blue;\">%2</div>"
                            "</div>"
                           ).arg(msg).arg(current_date);
            ui->posixshm_textBrowser->append(qmsg);
        }else
        {
            QMessageBox::warning(this, "ERROR", QString("POSIXSHM接收消息出错．错误码: %1．将关闭该IPC").arg(msg), QMessageBox::Ok);
            this->posixshm_stop_connect();
        }
    }
    else if(ipc_method == IPC_SYSTEMV_SHM)
    {
        if(type)
        {
            msg = msg.replace("<","&lt;"); //#6c757d
            msg = msg.replace(">","&gt;");
            msg = msg.replace(" ","&nbsp;");
            msg = msg.replace("\n","<br>");
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            qmsg = QString("<div>"
                            "<small>&nbsp;</small>"
                            "<p align=left style=\"font-weight: bold; color: orange;\">[SYSTEMVSHM_MSG]:</p>"
                            "<p align=left style='color: black'>%1</p>"
                            "<div align=right style=\"color:blue;\">%2</div>"
                            "</div>"
                           ).arg(msg).arg(current_date);
            ui->sysvshm_textBrowser->append(qmsg);
        }else
        {
            QMessageBox::warning(this, "ERROR", QString("SYSTEMVSHM接收消息出错．错误码: %1．将关闭该IPC").arg(msg), QMessageBox::Ok);
            this->sysvshm_stop_connect();
        }
    }
    else if(ipc_method == IPC_SOCKET_UDP)
    {
        if(type)
        {
            msg = msg.replace("<","&lt;"); //#6c757d
            msg = msg.replace(">","&gt;");
            msg = msg.replace(" ","&nbsp;");
            msg = msg.replace("\n","<br>");
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
            qmsg = QString("<div>"
                            "<small>&nbsp;</small>"
                            "<p align=left style=\"font-weight: bold; color: orange;\">[SOCKET_UDP_MSG]:</p>"
                            "<p align=left style='color: black'>%1</p>"
                            "<div align=right style=\"color:blue;\">%2</div>"
                            "</div>"
                           ).arg(msg).arg(current_date);
            ui->socketudp_textBrowser->append(qmsg);
        }else
        {
            QMessageBox::warning(this, "ERROR", QString("SOCKET_UDP接收消息出错．错误码: %1．将关闭该IPC").arg(msg), QMessageBox::Ok);
            this->socketudp_stop_connect();
        }
    }
}


bool MainWindow::fifo_start_connect()
{
    FIFO_CB *fifo_cb = new FIFO_CB;

    // 输入参数获取并检查
    QString qpath = ui->fifo_lineEdit01->text();
    if(qpath.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "path不能为空", QMessageBox::Ok);
        return false;
    }

    char *path = QStringToChar(qpath);
    bool b;
//    qDebug() << "s1 " << path << " " << qpath; //不能qdebug()输出，一输出，数据就变了．
    qDebug() << strlen(path);
    qDebug() << strlen("/tmp/fifo1");
//    b = init_fifo_cb(fifo_cb,IPC_MSG_RECEIVE, "/tmp/fifo1", strlen("/tmp/fifo1"));
    b = init_fifo_cb(fifo_cb,IPC_MSG_RECEIVE, path, strlen(path));
    qDebug() << "s2";
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(fifo_get_errcode(fifo_cb)), QMessageBox::Ok);
        return false;
    }

    ui->fifo_btn_enter->setText("关闭");
    ui->fifo_state_label->setText("状态：已开启");
    int ind = mapIPC[IPC_FIFO];
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)fifo_cb;
    this->ipc_ctx[ind].thread = new ReceiveThread();
    this->ipc_ctx[ind].thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_fifo_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::finished, this->ipc_ctx[ind].thread, &ReceiveThread::deleteLater);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].thread->start();
    this->ipc_ctx[ind].success = true;
    return true;
}

bool MainWindow::sysvmq_start_connect()
{
    SYSV_MQ_CB *sysv_mq_cb = new SYSV_MQ_CB;

    // 输入参数获取并检查
    QString qpath = ui->sysvmq_lineEdit01->text();
    if(qpath.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "path不能为空", QMessageBox::Ok);
        return false;
    }

    char path[PATH_MAX];
    QString2Char(qpath, path);
    qDebug() << "sysvmq " << strlen(path);
    bool b;
    b = init_sysv_mq_cb(sysv_mq_cb,IPC_MSG_RECEIVE, path, strlen(path));
//    b = init_sysv_mq_cb(sysv_mq_cb,IPC_MSG_RECEIVE, "sysvmq1", strlen("sysvmq1"));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_mq_get_errcode(sysv_mq_cb)), QMessageBox::Ok);
        return false;
    }

    ui->sysvmq_btn_enter->setText("关闭");
    ui->sysvmq_state_label->setText("状态：已开启");
    int ind = mapIPC[IPC_SYSTEMV_MQUEUE];
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)sysv_mq_cb;
    this->ipc_ctx[ind].thread = new ReceiveThread();
    this->ipc_ctx[ind].thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_sysv_mq_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::finished, this->ipc_ctx[ind].thread, &ReceiveThread::deleteLater);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].thread->start();
    this->ipc_ctx[ind].success = true;
    return true;
}

bool MainWindow::posixmq_start_connect()
{
    POSIX_MQ_CB *posix_mq_cb = new POSIX_MQ_CB;

    // 输入参数获取并检查
    QString qpath = ui->posixmq_lineEdit01->text();
    if(qpath.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "path不能为空", QMessageBox::Ok);
        return false;
    }

    char *path = QStringToChar(qpath);

    bool b;
    b = init_posix_mq_cb(posix_mq_cb,IPC_MSG_RECEIVE, path, strlen(path));
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_mq_get_errcode(posix_mq_cb)), QMessageBox::Ok);
        return false;
    }

    ui->posixmq_btn_enter->setText("关闭");
    ui->posixmq_state_label->setText("状态：已开启");
    int ind = mapIPC[IPC_POSIX_MQUEUE];
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)posix_mq_cb;
    this->ipc_ctx[ind].thread = new ReceiveThread();
    this->ipc_ctx[ind].thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_posix_mq_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::finished, this->ipc_ctx[ind].thread, &ReceiveThread::deleteLater);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].thread->start();
    this->ipc_ctx[ind].success = true;
    return true;
}

bool MainWindow::posixshm_start_connect()
{
    POSIX_SHM_CB *posix_shm_cb = new POSIX_SHM_CB;

    QString qpath1 = ui->posixshm_lineEdit01->text();
    if(qpath1.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "PosixSHM PATH不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath2 = ui->posixshm_lineEdit02->text();
    if(qpath2.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "Mutex1不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath3 = ui->posixshm_lineEdit03->text();
    if(qpath3.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "Mutex2不能为空", QMessageBox::Ok);
        return false;
    }

    char path1[PATH_MAX];
    char path2[PATH_MAX];
    char path3[PATH_MAX];
    QString2Char(qpath1, path1);
    QString2Char(qpath2, path2);
    QString2Char(qpath3, path3);

    qDebug() << strlen(path1) << " " << strlen(path2) << " " << strlen(path3);
    bool b;
    //b = init_posix_shm_cb(posix_shm_cb,IPC_MSG_RECEIVE, "/posixshm1", strlen("/posixshm1"),"mutex1", strlen("mutex1"), "mutex2", strlen("mutex2"));
    b = init_posix_shm_cb(posix_shm_cb, IPC_MSG_RECEIVE, path1, strlen(path1),path2, strlen(path2),path3, strlen(path3));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_shm_get_errcode(posix_shm_cb)), QMessageBox::Ok);
        return false;
    }

    ui->posixshm_btn_enter->setText("关闭");
    ui->posixshm_state_label->setText("状态：已开启");
    int ind = mapIPC[IPC_POSIX_SHM];
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)posix_shm_cb;
    this->ipc_ctx[ind].thread = new ReceiveThread();
    this->ipc_ctx[ind].thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_posix_shm_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::finished, this->ipc_ctx[ind].thread, &ReceiveThread::deleteLater);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].thread->start();
    this->ipc_ctx[ind].success = true;
    return true;
}

bool MainWindow::sysvshm_start_connect()
{
    SYSV_SHM_CB *sysv_shm_cb = new SYSV_SHM_CB;

    QString qpath1 = ui->sysvshm_lineEdit01->text();
    if(qpath1.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "SYSVSHM PATH不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath2 = ui->sysvshm_lineEdit02->text();
    if(qpath2.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "Mutex1不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath3 = ui->sysvshm_lineEdit03->text();
    if(qpath3.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "Mutex2不能为空", QMessageBox::Ok);
        return false;
    }

    char path1[PATH_MAX];
    char path2[PATH_MAX];
    char path3[PATH_MAX];
    QString2Char(qpath1, path1);
    QString2Char(qpath2, path2);
    QString2Char(qpath3, path3);

    qDebug() << strlen(path1) << " " << strlen(path2) << " " << strlen(path3);
    bool b;
    //b = init_sysv_shm_cb(sysv_shm_cb,IPC_MSG_RECEIVE, "/sysvshm1", strlen("/sysvshm1"),"mutex1", strlen("mutex1"), "mutex2", strlen("mutex2"));
    b = init_sysv_shm_cb(sysv_shm_cb, IPC_MSG_RECEIVE, path1, strlen(path1),path2, strlen(path2),path3, strlen(path3));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_shm_get_errcode(sysv_shm_cb)), QMessageBox::Ok);
        return false;
    }

    ui->sysvshm_btn_enter->setText("关闭");
    ui->sysvshm_state_label->setText("状态：已开启");
    int ind = mapIPC[IPC_SYSTEMV_SHM];
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)sysv_shm_cb;
    this->ipc_ctx[ind].thread = new ReceiveThread();
    this->ipc_ctx[ind].thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_sysv_shm_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::finished, this->ipc_ctx[ind].thread, &ReceiveThread::deleteLater);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].thread->start();
    this->ipc_ctx[ind].success = true;
    return true;
}

bool MainWindow::socketudp_start_connect()
{

    SOCKET_UDP_CB *socket_udp_cb = new SOCKET_UDP_CB;
    // 输入参数获取并检查
    QString qpath1 = ui->socketudp_lineEdit01->text();
    if(qpath1.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "socketudp 发送端ip地址不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath2 = ui->socketudp_lineEdit02->text();
    if(qpath2.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "socketudp 发送端端口不能为空", QMessageBox::Ok);
        return false;
    }


    char path1[PATH_MAX];
    ushort port1 = qpath2.toUShort();
    char path3[] = "127.0.0.1";
    ushort port2 = 0;
    QString2Char(qpath1, path1);

    bool b;

    b = init_socket_udp_cb(socket_udp_cb,IPC_MSG_RECEIVE, path3, strlen(path3),port2,path1, strlen(path1),port1);

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(socket_udp_get_errcode(socket_udp_cb)), QMessageBox::Ok);
        return false;
    }

    ui->socketudp_btn_enter->setText("关闭");
    ui->socketudp_state_label->setText("状态：已开启");
    int ind = mapIPC[IPC_SOCKET_UDP];
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)socket_udp_cb;
    this->ipc_ctx[ind].thread = new ReceiveThread();
    this->ipc_ctx[ind].thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this,&MainWindow::signal_socket_udp_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::finished, this->ipc_ctx[ind].thread, &ReceiveThread::deleteLater);
    connect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].thread->start();
    this->ipc_ctx[ind].success = true;
    return true;
}

bool MainWindow::fifo_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_FIFO];
    FIFO_CB *fifo_cb = (FIFO_CB *)this->ipc_ctx[ind].ipc_cb;

    emit signal_fifo_stop_thread();
    ui->fifo_btn_enter->setText("开启");
    ui->fifo_state_label->setText("状态：未开启");
    ui->fifo_textBrowser->clear();

    disconnect(this,&MainWindow::signal_fifo_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    disconnect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].success = false;
    this->ipc_ctx[ind].thread->terminate();
    b = close_fifo_cb(fifo_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(fifo_get_errcode(fifo_cb)), QMessageBox::Ok);
        return false;
    }
    delete fifo_cb;
    return true;
}

bool MainWindow::sysvmq_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SYSTEMV_MQUEUE];
    SYSV_MQ_CB *sysv_mq_cb = (SYSV_MQ_CB *)this->ipc_ctx[ind].ipc_cb;

    emit signal_sysv_mq_stop_thread();
    ui->sysvmq_btn_enter->setText("开启");
    ui->sysvmq_state_label->setText("状态：未开启");
    ui->sysvmq_textBrowser->clear();

    disconnect(this,&MainWindow::signal_sysv_mq_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    disconnect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].success = false;
    this->ipc_ctx[ind].thread->terminate();
    b = close_sysv_mq_cb(sysv_mq_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_mq_get_errcode(sysv_mq_cb)), QMessageBox::Ok);
        return false;
    }
    delete sysv_mq_cb;
    return true;
}

bool MainWindow::posixmq_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_POSIX_MQUEUE];
    POSIX_MQ_CB *posix_mq_cb = (POSIX_MQ_CB *)this->ipc_ctx[ind].ipc_cb;

    emit signal_posix_mq_stop_thread();
    ui->posixmq_btn_enter->setText("开启");
    ui->posixmq_state_label->setText("状态：未开启");
    ui->posixmq_textBrowser->clear();

    disconnect(this,&MainWindow::signal_posix_mq_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    disconnect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].success = false;
    this->ipc_ctx[ind].thread->terminate();
    b = close_posix_mq_cb(posix_mq_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_mq_get_errcode(posix_mq_cb)), QMessageBox::Ok);
        return false;
    }
    delete posix_mq_cb;
    return true;
}

bool MainWindow::posixshm_stop_connect()
{
    bool b=1;
    int ind = mapIPC[IPC_POSIX_SHM];
    POSIX_SHM_CB *posix_shm_cb = (POSIX_SHM_CB *)this->ipc_ctx[ind].ipc_cb;

    emit signal_posix_shm_stop_thread();
    ui->posixshm_btn_enter->setText("开启");
    ui->posixshm_state_label->setText("状态：未开启");
    ui->posixshm_textBrowser->clear();

    disconnect(this,&MainWindow::signal_posix_shm_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    disconnect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].success = false;
    this->ipc_ctx[ind].thread->terminate();
//    b = close_posix_shm_cb(posix_shm_cb);
//    if(!b){
//        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_shm_get_errcode(posix_shm_cb)), QMessageBox::Ok);
//        return false;
//    }
   // delete posix_shm_cb;
    return true;
}


bool MainWindow::sysvshm_stop_connect()
{
    bool b=1;
    int ind = mapIPC[IPC_SYSTEMV_SHM];
    SYSV_SHM_CB *sysv_shm_cb = (SYSV_SHM_CB *)this->ipc_ctx[ind].ipc_cb;
    emit signal_sysv_shm_stop_thread();
    ui->sysvshm_btn_enter->setText("开启");
    ui->sysvshm_state_label->setText("状态：未开启");
    ui->sysvshm_textBrowser->clear();
   // qDebug() << "sysvshm1";
    disconnect(this,&MainWindow::signal_sysv_shm_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    disconnect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
  //  qDebug() << "sysvshm2";
    this->ipc_ctx[ind].success = false;
    this->ipc_ctx[ind].thread->terminate();
   // qDebug() << "sysvshm3";
    //b = close_sysv_shm_cb(sysv_shm_cb);
  //  qDebug() << "sysvshm4";
//    if(!b){
//        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_shm_get_errcode(sysv_shm_cb)), QMessageBox::Ok);
//        return false;
//    }
    //delete sysv_shm_cb;
    qDebug() << "sysvshm5";
    return true;
}

bool MainWindow::socketudp_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SOCKET_UDP];
    SOCKET_UDP_CB *socket_udp_cb = (SOCKET_UDP_CB *)this->ipc_ctx[ind].ipc_cb;

    emit signal_socket_udp_stop_thread();
    ui->socketudp_btn_enter->setText("开启");
    ui->socketudp_state_label->setText("状态：未开启");
    ui->socketudp_textBrowser->clear();

    disconnect(this,&MainWindow::signal_socket_udp_stop_thread, this->ipc_ctx[ind].thread, &ReceiveThread::quit);
    disconnect(this->ipc_ctx[ind].thread, &ReceiveThread::signal_msg, this, &MainWindow::slot_common_receive_msg);
    this->ipc_ctx[ind].success = false;
    this->ipc_ctx[ind].thread->terminate();
    b = close_socket_udp_cb(socket_udp_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(socket_udp_get_errcode(socket_udp_cb)), QMessageBox::Ok);
        return false;
    }
    delete socket_udp_cb;
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}


char* MainWindow::QStringToChar(QString str)
{
    return str.toUtf8().data();
}

void MainWindow::QString2Char(QString src, char* dest)
{
    char *t = src.toUtf8().data();
    memcpy(dest, t, strlen(t));
    dest[strlen(t)] = '\0';
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int b = QMessageBox::question(this, "tip", "您确定要关闭吗？并关闭所有的开启的IPC");
    if(b == QMessageBox::Yes)
    {
        for(int i = 0; i < IPC_NUM; i++)
        {
            if(this->ipc_ctx[i].success == 1)
            {
                switch (ipc_ctx[i].ipc_cb->method) {
                    case IPC_SYSTEMV_MQUEUE:
                    {
                        qDebug() << "正在关闭:IPC_SYSV_MQ";
                        this->sysvmq_stop_connect();
                        break;
                    }
                    case IPC_POSIX_MQUEUE:
                    {
                        qDebug() << "正在关闭:IPC_POSIX_MQ";
                        this->posixmq_stop_connect();
                        break;
                    }
                    case IPC_FIFO:
                    {
                        qDebug() << "正在关闭:IPC_FIFO";
                        this->fifo_stop_connect();
                        break;
                    }
                    case IPC_POSIX_SHM:
                    {
                        qDebug() << "正在关闭:IPC_POSIX_SHM";
                        this->posixshm_stop_connect();
                        break;
                    }
                    case IPC_SYSTEMV_SHM:
                    {
                        qDebug() << "正在关闭:IPC_SYSV_SHM";
                        this->sysvshm_stop_connect();
                        break;
                    }
                    case IPC_SOCKET_UDP:
                    {
                        qDebug() << "正在关闭:IPC_SOCKET_UDP";
                        this->socketudp_stop_connect();
                        break;
                    }
                    case IPC_SOCKET_TCP:
                    {
                        qDebug() << "正在关闭:IPC_SOCKET_TCP";
                        this->sockettcp_stop_connect();
                        break;
                    }
                }
            }
        }
        event->accept();
    }else{
        event->ignore();
    }
}
