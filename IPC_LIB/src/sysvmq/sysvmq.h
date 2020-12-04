//
// Created by kervias on 20-8-11.
//
#ifndef SYSTEMV_MQ_SYSMQ_H
#define SYSTEMV_MQ_SYSMQ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/model.h"



bool  init_sysv_mq_cb(SYSV_MQ_CB *sysv_mq_cb, IPC_SEND_OR_RECEIVE type, char *path, size_t len);

bool
close_sysv_mq_cb(SYSV_MQ_CB *sysv_mq_cb);

bool
sysv_mq_send_msg(SYSV_MQ_CB *sysv_mq_cb, SYSV_MQ_MSG *sysv_mq_msg);

bool
sysv_mq_receive_msg(SYSV_MQ_CB *sysv_mq_cb, SYSV_MQ_MSG *sysv_mq_msg);

int sysv_mq_get_errcode(SYSV_MQ_CB *sysv_mq_cb);


#ifdef __cplusplus
}
#endif
#endif //SYSTEMV_MQ_SYSMQ_H
