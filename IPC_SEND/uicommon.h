#ifndef UICOMMON_H
#define UICOMMON_H

#include <QString>
struct common_msg
{
    char type; // 'A':连接成功消息 'B'：连接失败消息　＇C＇: 发送消息成功　'D': 发送消息失败
    int tmp1; // msg_len or err_code
    int tmp2; // 错误消息中的errno
    QString msg; //　消息体
    common_msg(char type='0', int t1=0, int t2=0, QString msg=""):type(type),tmp1(t1),tmp2(t2),msg(msg){}
};

#endif // UICOMMON_H
