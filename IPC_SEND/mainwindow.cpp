#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QString>
#include <QSizePolicy>
#include <QDebug>
#include <QMessageBox>
#include "sysvmq/sysvmq.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<common_msg>("common_msg");
    mapIPC[IPC_FIFO] = 0;
    mapIPC[IPC_POSIX_MQUEUE] = 1;
    mapIPC[IPC_POSIX_SHM] = 2;
    mapIPC[IPC_SYSTEMV_MQUEUE] = 3;
    mapIPC[IPC_SYSTEMV_SHM] = 4;
    mapIPC[IPC_SOCKET_UDP] = 5;
    mapIPC[IPC_SOCKET_TCP] = 6;
    socket_tcp_state = 0;
    socket_tcp_thread = nullptr;
    this->initMainWindow();
    this->initEventBind();
}

void MainWindow::initEventBind()
{
    // 界面逻辑控制
    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &MainWindow::slot_currentItemChanged);

    // fifo相关
    connect(ui->fifo_btn_enter,&QPushButton::released, this, &MainWindow::slot_fifo_btn_enter);
    connect(ui->fifo_btn_send, &QPushButton::released, this, &MainWindow::slot_fifo_send_msg);

    // sysv_mq　相关
    connect(ui->sysvmq_btn_enter, &QPushButton::released, this, &MainWindow::slot_sysvmq_btn_enter);
    connect(ui->sysvmq_btn_send, &QPushButton::released, this, &MainWindow::slot_sysvmq_send_msg);

    //　posix_mq 相关
    connect(ui->posixmq_btn_enter, &QPushButton::released, this, &MainWindow::slot_posixmq_btn_enter);
    connect(ui->posixmq_btn_send, &QPushButton::released, this, &MainWindow::slot_posixmq_send_msg);

    // posix_shm 相关
    connect(ui->posixshm_btn_enter, &QPushButton::released, this, &MainWindow::slot_posixshm_btn_enter);
    connect(ui->posixshm_btn_send, &QPushButton::released, this, &MainWindow::slot_posixshm_send_msg);

    // sysv_shm　相关
    connect(ui->sysvshm_btn_enter, &QPushButton::released, this, &MainWindow::slot_sysvshm_btn_enter);
    connect(ui->sysvshm_btn_send, &QPushButton::released, this, &MainWindow::slot_sysvshm_send_msg);

    // socket_udp　相关
    connect(ui->socketudp_btn_enter, &QPushButton::released, this, &MainWindow::slot_socketudp_btn_enter);
    connect(ui->socketudp_btn_send, &QPushButton::released, this, &MainWindow::slot_socketudp_send_msg);

    // socket_tcp 相关
    connect(ui->sockettcp_btn_enter, &QPushButton::released, this, &MainWindow::slot_sockettcp_btn_enter);
    connect(ui->sockettcp_btn_send, &QPushButton::released, this, &MainWindow::slot_sockettcp_send_msg);

}

// 初始化窗口
void MainWindow::initMainWindow()
{

    this->setWindowTitle("发送进程");
    ui->splitter->setStretchFactor(0,35);
    ui->splitter->setStretchFactor(1,65);

    ui->fifo_splitter->setStretchFactor(0,3);
    ui->fifo_splitter->setStretchFactor(1,1);

    ui->posixmq_splitter->setStretchFactor(0,3);
    ui->posixmq_splitter->setStretchFactor(1,1);

    ui->sysvmq_splitter->setStretchFactor(0,3);
    ui->sysvmq_splitter->setStretchFactor(1,1);

    ui->posixshm_splitter->setStretchFactor(0,3);
    ui->posixshm_splitter->setStretchFactor(1,1);

    ui->sysvshm_splitter->setStretchFactor(0,3);
    ui->sysvshm_splitter->setStretchFactor(1,1);

    ui->socketudp_splitter->setStretchFactor(0,3);
    ui->socketudp_splitter->setStretchFactor(1,1);

    ui->sockettcp_splitter->setStretchFactor(0,3);
    ui->sockettcp_splitter->setStretchFactor(1,1);

    ui->posixmq_spinBox->setValue(1);
    ui->sysvmq_spinBox->setValue(1);


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
        if(b) QMessageBox::information(this, "INFO", "FIFO 关闭成功！", QMessageBox::Ok);
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
        if(b) QMessageBox::information(this, "INFO", "Posix MQ开启成功！", QMessageBox::Ok);
    }else{
        b = this->posixmq_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "Posix MQ关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_posixshm_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_POSIX_SHM]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->posixshm_start_connect();
        if(b) QMessageBox::information(this, "INFO", "Posix SHM开启成功！", QMessageBox::Ok);
    }else{
        b = this->posixshm_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "Posix SHM关闭成功！", QMessageBox::Ok);
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

void MainWindow::slot_socketudp_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_SOCKET_UDP]].success) //已经初始化了，再次点击将取消连接
    {
        b = this->socketudp_start_connect();
        if(b) QMessageBox::information(this, "INFO", "SOCKET UDP开启成功！", QMessageBox::Ok);
    }else{
        b = this->socketudp_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "SOCKET UDP关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_sockettcp_btn_enter()
{
    bool b;
    if(!this->ipc_ctx[mapIPC[IPC_SOCKET_TCP]].success) //已经初始化了，再次点击将取消连接
    {
        if(this->socket_tcp_state == 1)
        {
            QMessageBox::information(this, "INFO", "SOCKET TCP正在连接中！", QMessageBox::Ok);
            return;
        }
        b = this->sockettcp_start_connect();
//        if(b) QMessageBox::information(this, "INFO", "SOCKET UDP开启成功！", QMessageBox::Ok);
    }else{
        b = this->sockettcp_stop_connect();
        if(b) QMessageBox::information(this, "INFO", "SOCKET TCP关闭成功！", QMessageBox::Ok);
    }
}

void MainWindow::slot_sockettcp_send_msg()
{
    int ind = this->mapIPC[IPC_SOCKET_TCP];
    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","SOCKET_TCP尚未开启", QMessageBox::Ok);
        return;
    }

    if(this->socket_tcp_state == 3)
    {
        //检查输入合法性
        QString qmsg = ui->sockettcp_textEdit->toPlainText();
        QString raw_qmsg = qmsg;
        if(qmsg.length() == 0)
        {
            QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
            return;
        }
        qmsg = qmsg.replace("<","&lt;"); //#6c757d
        qmsg = qmsg.replace(">","&gt;");
        qmsg = qmsg.replace(" ","&nbsp;");
        qmsg = qmsg.replace("\n","<br>");
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
        qmsg = QString("<div>"
                        "<small>&nbsp;</small>"
                        "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[SOCKET_TCP_MSG]:</p>"
                        "<p align=left style='color: black'>%1</p>"
                        "<div align=right style=\"color:blue;\">%2</div>"
                        "</div>"
                       ).arg(qmsg).arg(current_date);
        ui->sockettcp_textBrowser->append(qmsg);
        ui->sockettcp_textEdit->clear();
        this->socket_tcp_state = 2;
        emit signal_sockettcp_send_msg(common_msg('E',0,0,raw_qmsg));
        this->socket_tcp_thread->start();
    }else{
        QMessageBox::warning(this, "警告","SOCKET_TCP上一消息尚未发送完毕", QMessageBox::Ok);
        return;
    }

}

void MainWindow::slot_sockettcp_tip_msg(common_msg cmsg)
{
    if(cmsg.type == 'A') //连接成功
    {
        this->socket_tcp_state = 3;
        ui->sockettcp_btn_enter->setText("关闭");
        ui->sockettcp_state_label->setText("状态：已连接");
        this->ipc_ctx[mapIPC[IPC_SOCKET_TCP]].success = true;
    }else if(cmsg.type == 'B')//连接失败
    {
        this->socket_tcp_state = 0;
        QMessageBox::warning(this, "ERROR", QString("%1 \n错误码: %2\n errno:%3").arg(cmsg.msg).arg(cmsg.tmp1).arg(cmsg.tmp2), QMessageBox::Ok);
        this->sockettcp_stop_connect();
    }else if(cmsg.type == 'C')//发送消息成功
    {
        this->socket_tcp_state = 3;
    }else if(cmsg.type == 'D')//发送消息失败
    {
        QMessageBox::warning(this, "警告",QString("SOCKET_TCP发送失败\n%1 \n错误码: %2\n errno:%3").arg(cmsg.msg).arg(cmsg.tmp1).arg(cmsg.tmp2), QMessageBox::Ok);
        this->sockettcp_stop_connect();
    }
}


char* MainWindow::QStringToChar(QString str)
{
    return str.toUtf8().data();
}

bool MainWindow::fifo_start_connect()
{
    FIFO_CB *fifo_cb = new FIFO_CB;
    int ind = mapIPC[IPC_FIFO];
    // 输入参数获取并检查
    QString qpath = ui->fifo_lineEdit01->text();
    if(qpath.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "path不能为空", QMessageBox::Ok);
        return false;
    }

    char *path = QStringToChar(qpath);

    bool b;
    b = init_fifo_cb(fifo_cb,IPC_MSG_SEND, path, strlen(path));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(fifo_get_errcode(fifo_cb)), QMessageBox::Ok);
        return false;
    }

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)fifo_cb;
    this->ipc_ctx[ind].success = true;

    ui->fifo_btn_enter->setText("关闭");
    ui->fifo_textBrowser->clear();
    ui->fifo_textBrowser->clearHistory();
    ui->fifo_state_label->setText("状态：已开启");
    return true;
}

bool MainWindow::fifo_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_FIFO];
    FIFO_CB *fifo_cb = (FIFO_CB *)this->ipc_ctx[ind].ipc_cb;
    ui->fifo_btn_enter->setText("开启");
    ui->fifo_state_label->setText("状态：未开启");
    ui->fifo_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    b = close_fifo_cb(fifo_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(fifo_get_errcode(fifo_cb)), QMessageBox::Ok);
        return false;
    }
    delete fifo_cb;
    return true;
}

bool MainWindow::sysvmq_start_connect()
{
    SYSV_MQ_CB *sysv_mq_cb = new SYSV_MQ_CB;
    int ind = mapIPC[IPC_SYSTEMV_MQUEUE];
    // 输入参数获取并检查
    QString qpath = ui->sysvmq_lineEdit01->text();
    if(qpath.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "path不能为空", QMessageBox::Ok);
        return false;
    }

    char *path = QStringToChar(qpath);

    bool b;
    b = init_sysv_mq_cb(sysv_mq_cb,IPC_MSG_SEND, path, strlen(path));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_mq_get_errcode(sysv_mq_cb)), QMessageBox::Ok);
        return false;
    }

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)sysv_mq_cb;
    this->ipc_ctx[ind].success = true;

    ui->sysvmq_btn_enter->setText("关闭");
    ui->sysvmq_textBrowser->clear();
    ui->sysvmq_textBrowser->clearHistory();
    ui->sysvmq_state_label->setText("状态：已开启");
    return true;
}

bool MainWindow::sysvmq_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SYSTEMV_MQUEUE];
    SYSV_MQ_CB *sysv_mq_cb = (SYSV_MQ_CB *)this->ipc_ctx[ind].ipc_cb;
    ui->sysvmq_btn_enter->setText("开启");
    ui->sysvmq_state_label->setText("状态：未开启");
    ui->sysvmq_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    b = close_sysv_mq_cb(sysv_mq_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_mq_get_errcode(sysv_mq_cb)), QMessageBox::Ok);
        return false;
    }
    delete sysv_mq_cb;
    return true;
}

bool MainWindow::posixmq_start_connect()
{

    POSIX_MQ_CB *posix_mq_cb = new POSIX_MQ_CB;
    int ind = mapIPC[IPC_POSIX_MQUEUE];
    // 输入参数获取并检查
    QString qpath = ui->posixmq_lineEdit01->text();
    if(qpath.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "path不能为空", QMessageBox::Ok);
        return false;
    }

    char *path = QStringToChar(qpath);

    bool b;
    b = init_posix_mq_cb(posix_mq_cb,IPC_MSG_SEND, path, strlen(path));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_mq_get_errcode(posix_mq_cb)), QMessageBox::Ok);
        return false;
    }

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)posix_mq_cb;
    this->ipc_ctx[ind].success = true;

    ui->posixmq_btn_enter->setText("关闭");
    ui->posixmq_textBrowser->clear();
    ui->posixmq_textBrowser->clearHistory();
    ui->posixmq_state_label->setText("状态：已开启");
    return true;
}

bool MainWindow::posixmq_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_POSIX_MQUEUE];
    POSIX_MQ_CB *posix_mq_cb = (POSIX_MQ_CB *)this->ipc_ctx[ind].ipc_cb;
    ui->posixmq_btn_enter->setText("开启");
    ui->posixmq_state_label->setText("状态：未开启");
    ui->posixmq_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    b = close_posix_mq_cb(posix_mq_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_mq_get_errcode(posix_mq_cb)), QMessageBox::Ok);
        return false;
    }
    delete posix_mq_cb;
    return true;
}


bool MainWindow::posixshm_start_connect()
{

    POSIX_SHM_CB *posix_shm_cb = new POSIX_SHM_CB;
    int ind = mapIPC[IPC_POSIX_SHM];
    // 输入参数获取并检查
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
//    b = init_posix_shm_cb(posix_shm_cb,IPC_MSG_SEND, "/posixshm1", strlen("/posixshm1"),"mutex1", strlen("mutex1"), "mutex2", strlen("mutex2"));

    b = init_posix_shm_cb(posix_shm_cb,IPC_MSG_SEND, path1, strlen(path1),path2, strlen(path2),path3, strlen(path3));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_shm_get_errcode(posix_shm_cb)), QMessageBox::Ok);
        return false;
    }

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)posix_shm_cb;
    this->ipc_ctx[ind].success = true;

    ui->posixshm_btn_enter->setText("关闭");
    ui->posixshm_textBrowser->clear();
    ui->posixshm_textBrowser->clearHistory();
    ui->posixshm_state_label->setText("状态：已开启");
    qDebug() << "here";
    return true;
}

bool MainWindow::sysvshm_start_connect()
{

    SYSV_SHM_CB *sysv_shm_cb = new SYSV_SHM_CB;
    int ind = mapIPC[IPC_SYSTEMV_SHM];
    // 输入参数获取并检查
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
//    b = init_sysv_shm_cb(sysv_shm_cb,IPC_MSG_SEND, "/sysvshm1", strlen("/sysvshm1"),"mutex1", strlen("mutex1"), "mutex2", strlen("mutex2"));

    b = init_sysv_shm_cb(sysv_shm_cb,IPC_MSG_SEND, path1, strlen(path1),path2, strlen(path2),path3, strlen(path3));

    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_shm_get_errcode(sysv_shm_cb)), QMessageBox::Ok);
        return false;
    }

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)sysv_shm_cb;
    this->ipc_ctx[ind].success = true;

    ui->sysvshm_btn_enter->setText("关闭");
    ui->sysvshm_textBrowser->clear();
    ui->sysvshm_textBrowser->clearHistory();
    ui->sysvshm_state_label->setText("状态：已开启");
    qDebug() << "here";
    return true;
}


bool MainWindow::posixshm_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_POSIX_SHM];
    POSIX_SHM_CB *posix_shm_cb = (POSIX_SHM_CB *)this->ipc_ctx[ind].ipc_cb;
    ui->posixshm_btn_enter->setText("开启");
    ui->posixshm_state_label->setText("状态：未开启");
    ui->posixshm_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    b = close_posix_shm_cb(posix_shm_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(posix_shm_get_errcode(posix_shm_cb)), QMessageBox::Ok);
        return false;
    }
    delete posix_shm_cb;
    return true;
}


bool MainWindow::sysvshm_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SYSTEMV_SHM];
    SYSV_SHM_CB *sysv_shm_cb = (SYSV_SHM_CB *)this->ipc_ctx[ind].ipc_cb;
    ui->sysvshm_btn_enter->setText("开启");
    ui->sysvshm_state_label->setText("状态：未开启");
    ui->sysvshm_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    b = close_sysv_shm_cb(sysv_shm_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(sysv_shm_get_errcode(sysv_shm_cb)), QMessageBox::Ok);
        return false;
    }
    delete sysv_shm_cb;
    return true;
}

bool MainWindow::socketudp_start_connect()
{

    SOCKET_UDP_CB *socket_udp_cb = new SOCKET_UDP_CB;
    int ind = mapIPC[IPC_SOCKET_UDP];
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
    QString qpath3 = ui->socketudp_lineEdit03->text();
    if(qpath3.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "socketudp 接收端ip地址不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath4 = ui->socketudp_lineEdit04->text();
    if(qpath4.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "socketudp 接收端端口不能为空", QMessageBox::Ok);
        return false;
    }

    char path1[PATH_MAX];
    ushort port1 = qpath2.toUShort();
    char path3[PATH_MAX];
    ushort port2 = qpath4.toUShort();
    QString2Char(qpath1, path1);
    QString2Char(qpath3, path3);


    bool b;

    b = init_socket_udp_cb(socket_udp_cb,IPC_MSG_SEND, path1, strlen(path1),port1,path3, strlen(path3),port2);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(socket_udp_get_errcode(socket_udp_cb)), QMessageBox::Ok);
        return false;
    }
    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)socket_udp_cb;
    this->ipc_ctx[ind].success = true;

    ui->socketudp_btn_enter->setText("关闭");
    ui->socketudp_textBrowser->clear();
    ui->socketudp_textBrowser->clearHistory();
    ui->socketudp_state_label->setText("状态：已开启");
    return true;
}

bool MainWindow::socketudp_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SOCKET_UDP];
    SOCKET_UDP_CB *socket_udp_cb = (SOCKET_UDP_CB *)this->ipc_ctx[ind].ipc_cb;
    ui->socketudp_btn_enter->setText("开启");
    ui->socketudp_state_label->setText("状态：未开启");
    ui->socketudp_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    b = close_socket_udp_cb(socket_udp_cb);
    if(!b){
        QMessageBox::warning(this, "ERROR", QString("错误码: %1").arg(socket_udp_get_errcode(socket_udp_cb)), QMessageBox::Ok);
        return false;
    }
    delete socket_udp_cb;
    return true;
}

bool MainWindow::sockettcp_start_connect()
{
    ui->sockettcp_textBrowser->clear();
    ui->sockettcp_textBrowser->clearHistory();
    SOCKET_TCP_CB *socket_tcp_cb = new SOCKET_TCP_CB;
    int ind = mapIPC[IPC_SOCKET_TCP];
    // 输入参数获取并检查
    QString qpath1 = ui->sockettcp_lineEdit01->text();
    if(qpath1.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "sockettcp 发送端ip地址不能为空", QMessageBox::Ok);
        return false;
    }
    QString qpath2 = ui->sockettcp_lineEdit02->text();
    if(qpath2.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "sockettcp 发送端端口不能为空", QMessageBox::Ok);
        return false;
    }
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

    char path1[PATH_MAX];
    ushort port1 = qpath2.toUShort();
    char path3[PATH_MAX];
    ushort port2 = qpath4.toUShort();
    QString2Char(qpath1, path1);
    QString2Char(qpath3, path3);

    this->ipc_ctx[ind].ipc_cb = (IPC_CB *)socket_tcp_cb;
    ui->sockettcp_state_label->setText("状态：正在连接server...");
    this->socket_tcp_state = 1;
    this->socket_tcp_thread = new SocketTCPThread(qpath1,port1,qpath3,port2);
    this->socket_tcp_thread->set_ipc_cb(this->ipc_ctx[ind].ipc_cb);
    connect(this->socket_tcp_thread, &SocketTCPThread::signal_msg, this, &MainWindow::slot_sockettcp_tip_msg);
    connect(this, &MainWindow::signal_sockettcp_send_msg, this->socket_tcp_thread, &SocketTCPThread::slot_send_msg);
    this->socket_tcp_thread->start();
    return true;
}

bool MainWindow::sockettcp_stop_connect()
{
    bool b;
    int ind = mapIPC[IPC_SOCKET_TCP];
    SOCKET_TCP_CB *socket_tcp_cb = (SOCKET_TCP_CB *)this->ipc_ctx[ind].ipc_cb;

    ui->sockettcp_textBrowser->clear();
    this->ipc_ctx[ind].success = false;
    disconnect(this->socket_tcp_thread, &SocketTCPThread::signal_msg, this, &MainWindow::slot_sockettcp_tip_msg);
    disconnect(this, &MainWindow::signal_sockettcp_send_msg, this->socket_tcp_thread, &SocketTCPThread::slot_send_msg);
    b = close_socket_tcp_cb(socket_tcp_cb);

    this->socket_tcp_state = 0;
//    this->socket_tcp_thread->terminate();
    delete this->socket_tcp_thread;
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

void MainWindow::slot_fifo_send_msg()
{
    int ind = mapIPC[IPC_FIFO];

    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","FIFO尚未开启", QMessageBox::Ok);
        return;
    }
    bool b;
    FIFO_CB *fifo_cb = (FIFO_CB *)this->ipc_ctx[ind].ipc_cb;

    //检查输入合法性
    QString qmsg = ui->fifo_textEdit->toPlainText();
    if(qmsg.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
        return;
    }

    // 发送消息
    FIFO_MSG fifo_msg;
    char *msg = QStringToChar(qmsg);
    memcpy(fifo_msg.msg, msg, strlen(msg));

    fifo_msg.len = strlen(msg);

    b = fifo_send_msg(fifo_cb, &fifo_msg);
    if(!b){
        QMessageBox::warning(this, "发送失败", QString("发送消息失败，错误码: %1").arg(fifo_get_errcode(fifo_cb)), QMessageBox::Ok);
        return;
    }

    qmsg = qmsg.replace("<","&lt;"); //#6c757d
    qmsg = qmsg.replace(">","&gt;");
    qmsg = qmsg.replace(" ","&nbsp;");
    qmsg = qmsg.replace("\n","<br>");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    qmsg = QString("<div>"
                    "<small>&nbsp;</small>"
                    "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[FIFO_MSG]:</p>"
                    "<p align=left style='color: black'>%1</p>"
                    "<div align=right style=\"color:blue;\">%2</div>"
                    "</div>"
                   ).arg(qmsg).arg(current_date);
    ui->fifo_textBrowser->append(qmsg);
    ui->fifo_textEdit->clear();
}



void MainWindow::slot_sysvmq_send_msg()
{
    int ind = mapIPC[IPC_SYSTEMV_MQUEUE];

    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","SystemV消息队列尚未开启", QMessageBox::Ok);
        return;
    }
    bool b;
    SYSV_MQ_CB *sysv_mq_cb = (SYSV_MQ_CB *)this->ipc_ctx[ind].ipc_cb;

    //检查输入合法性
    QString qmsg = ui->sysvmq_textEdit->toPlainText();
    if(qmsg.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
        return;
    }

    unsigned int mtype = ui->sysvmq_spinBox->value();
    // 发送消息
    SYSV_MQ_MSG sysv_mq_msg;
    char *msg = QStringToChar(qmsg);
    memcpy(sysv_mq_msg.msg, msg, strlen(msg));

    sysv_mq_msg.len = strlen(msg);
    sysv_mq_msg.mtype = mtype;

    b = sysv_mq_send_msg(sysv_mq_cb, &sysv_mq_msg);
    if(!b){
        QMessageBox::warning(this, "发送失败", QString("发送消息失败，错误码: %1").arg(sysv_mq_get_errcode(sysv_mq_cb)), QMessageBox::Ok);
        return;
    }

    qmsg = qmsg.replace("<","&lt;"); //#6c757d
    qmsg = qmsg.replace(">","&gt;");
    qmsg = qmsg.replace(" ","&nbsp;");
    qmsg = qmsg.replace("\n","<br>");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    qmsg = QString("<div>"
                    "<small>&nbsp;</small>"
                    "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[SYSV_MQ_MSG]: - 优先级[%2]</p>"
                    "<p align=left style='color: black'>%1</p>"
                    "<div align=right style=\"color:blue;\">%3</div>"
                    "</div>"
                   ).arg(qmsg).arg(mtype).arg(current_date);
    ui->sysvmq_textBrowser->append(qmsg);
    ui->sysvmq_textEdit->clear();
}


void MainWindow::slot_posixmq_send_msg()
{
    int ind = mapIPC[IPC_POSIX_MQUEUE];

    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","POSIX消息队列尚未开启", QMessageBox::Ok);
        return;
    }
    bool b;
    POSIX_MQ_CB *posix_mq_cb = (POSIX_MQ_CB *)this->ipc_ctx[ind].ipc_cb;

    //检查输入合法性
    QString qmsg = ui->posixmq_textEdit->toPlainText();
    if(qmsg.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
        return;
    }

    unsigned int mtype = ui->posixmq_spinBox->value();

    // 发送消息
    POSIX_MQ_MSG posix_mq_msg;
    char *msg = QStringToChar(qmsg);
    memcpy(posix_mq_msg.msg, msg, strlen(msg));

    posix_mq_msg.len = strlen(msg);
    posix_mq_msg.mtype = mtype;

    b = posix_mq_send_msg(posix_mq_cb, &posix_mq_msg);
    if(!b){
        QMessageBox::warning(this, "发送失败", QString("发送消息失败，错误码: %1").arg(posix_mq_get_errcode(posix_mq_cb)), QMessageBox::Ok);
        return;
    }

    qmsg = qmsg.replace("<","&lt;"); //#6c757d
    qmsg = qmsg.replace(">","&gt;");
    qmsg = qmsg.replace(" ","&nbsp;");
    qmsg = qmsg.replace("\n","<br>");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    qmsg = QString("<div>"
                    "<small>&nbsp;</small>"
                    "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[POSIX_MQ_MSG]: - 优先级[%2]</p>"
                    "<p align=left style='color: black'>%1</p>"
                    "<div align=right style=\"color:blue;\">%3</div>"
                    "</div>"
                   ).arg(qmsg).arg(mtype).arg(current_date);
    ui->posixmq_textBrowser->append(qmsg);
    ui->posixmq_textEdit->clear();
}

void MainWindow::slot_posixshm_send_msg()
{
    int ind = mapIPC[IPC_POSIX_SHM];

    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","POSIXSHM尚未开启", QMessageBox::Ok);
        return;
    }
    bool b;
    POSIX_SHM_CB *posix_shm_cb = (POSIX_SHM_CB *)this->ipc_ctx[ind].ipc_cb;

    //检查输入合法性
    QString qmsg = ui->posixshm_textEdit->toPlainText();
    if(qmsg.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
        return;
    }

    // 发送消息
    POSIX_SHM_MSG posix_shm_msg;
    char *msg = QStringToChar(qmsg);
    memcpy(posix_shm_msg.msg, msg, strlen(msg));
    posix_shm_msg.len = strlen(msg);
    qDebug() << posix_shm_msg.len;
    b = posix_shm_send_msg(posix_shm_cb, &posix_shm_msg);
    if(!b){
        QMessageBox::warning(this, "发送失败", QString("发送消息失败，错误码: %1").arg(posix_shm_get_errcode(posix_shm_cb)), QMessageBox::Ok);
        return;
    }

    qmsg = qmsg.replace("<","&lt;"); //#6c757d
    qmsg = qmsg.replace(">","&gt;");
    qmsg = qmsg.replace(" ","&nbsp;");
    qmsg = qmsg.replace("\n","<br>");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    qmsg = QString("<div>"
                    "<small>&nbsp;</small>"
                    "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[POSIX_SHM_MSG]:</p>"
                    "<p align=left style='color: black'>%1</p>"
                    "<div align=right style=\"color:blue;\">%2</div>"
                    "</div>"
                   ).arg(qmsg).arg(current_date);
    ui->posixshm_textBrowser->append(qmsg);
    ui->posixshm_textEdit->clear();
}


void MainWindow::slot_sysvshm_send_msg()
{
    int ind = mapIPC[IPC_SYSTEMV_SHM];

    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","SYSTEMV消息队列尚未开启", QMessageBox::Ok);
        return;
    }
    bool b;
    SYSV_SHM_CB *sysv_shm_cb = (SYSV_SHM_CB *)this->ipc_ctx[ind].ipc_cb;

    //检查输入合法性
    QString qmsg = ui->sysvshm_textEdit->toPlainText();
    if(qmsg.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
        return;
    }

    // 发送消息
    SYSV_SHM_MSG sysv_shm_msg;
    char *msg = QStringToChar(qmsg);
    memcpy(sysv_shm_msg.msg, msg, strlen(msg));

    sysv_shm_msg.len = strlen(msg);
    qDebug() << sysv_shm_msg.len;
    b = sysv_shm_send_msg(sysv_shm_cb, &sysv_shm_msg);
    if(!b){
        QMessageBox::warning(this, "发送失败", QString("发送消息失败，错误码: %1").arg(sysv_shm_get_errcode(sysv_shm_cb)), QMessageBox::Ok);
        return;
    }

    qmsg = qmsg.replace("<","&lt;"); //#6c757d
    qmsg = qmsg.replace(">","&gt;");
    qmsg = qmsg.replace(" ","&nbsp;");
    qmsg = qmsg.replace("\n","<br>");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    qmsg = QString("<div>"
                    "<small>&nbsp;</small>"
                    "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[SYSV_SHM_MSG]:</p>"
                    "<p align=left style='color: black'>%1</p>"
                    "<div align=right style=\"color:blue;\">%2</div>"
                    "</div>"
                   ).arg(qmsg).arg(current_date);
    ui->sysvshm_textBrowser->append(qmsg);
    ui->sysvshm_textEdit->clear();
}

void MainWindow::slot_socketudp_send_msg()
{
    int ind = mapIPC[IPC_SOCKET_UDP];

    if(this->ipc_ctx[ind].success == false)
    {
        QMessageBox::warning(this, "警告","SOCKET UDP尚未开启", QMessageBox::Ok);
        return;
    }
    bool b;
    SOCKET_UDP_CB *socket_udp_cb = (SOCKET_UDP_CB *)this->ipc_ctx[ind].ipc_cb;

    //检查输入合法性
    QString qmsg = ui->socketudp_textEdit->toPlainText();
    if(qmsg.length() == 0)
    {
        QMessageBox::warning(this, "ERROR", "消息不能为空", QMessageBox::Ok);
        return;
    }

    // 发送消息
    SOCKET_UDP_MSG socket_udp_msg;
    char *msg = QStringToChar(qmsg);
    memcpy(socket_udp_msg.msg, msg, strlen(msg));

    socket_udp_msg.len = strlen(msg);
    //qDebug() << socket_udp_msg.len;
    b = socket_udp_send_msg(socket_udp_cb, &socket_udp_msg);
    if(!b){
        QMessageBox::warning(this, "发送失败", QString("发送消息失败，错误码: %1").arg(socket_udp_get_errcode(socket_udp_cb)), QMessageBox::Ok);
        return;
    }

    qmsg = qmsg.replace("<","&lt;"); //#6c757d
    qmsg = qmsg.replace(">","&gt;");
    qmsg = qmsg.replace(" ","&nbsp;");
    qmsg = qmsg.replace("\n","<br>");
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    qmsg = QString("<div>"
                    "<small>&nbsp;</small>"
                    "<p align=left style=\"font-weight: bold; color: deepskyblue;\">[SOCKET_UDP_MSG]:</p>"
                    "<p align=left style='color: black'>%1</p>"
                    "<div align=right style=\"color:blue;\">%2</div>"
                    "</div>"
                   ).arg(qmsg).arg(current_date);
    ui->socketudp_textBrowser->append(qmsg);
    ui->socketudp_textEdit->clear();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    int b = QMessageBox::question(this, "tip", "您确定要关闭吗？并关闭所有开启的IPC");
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
                        qDebug() << "正在关闭:IPC_POSIX_SHM";
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

void MainWindow::QString2Char(QString src, char* dest)
{
    char *t = src.toUtf8().data();
    memcpy(dest, t, strlen(t));
    dest[strlen(t)] = '\0';
}


MainWindow::~MainWindow()
{
    delete ui;
}
