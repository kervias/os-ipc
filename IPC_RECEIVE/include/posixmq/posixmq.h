//
// Created by kervias on 20-8-11.
//

#ifndef POSIX_NQ_POSIXMQ_H
#define POSIX_NQ_POSIXMQ_H

#ifdef __cplusplus
extern "C" {
#endif
#include "../common/model.h"


bool init_posix_mq_cb(POSIX_MQ_CB *posix_mq_cb, IPC_SEND_OR_RECEIVE type, char *path, size_t len);

bool
close_posix_mq_cb(POSIX_MQ_CB *posix_mq_cb);

bool
posix_mq_send_msg(POSIX_MQ_CB *posix_mq_cb, POSIX_MQ_MSG *posix_mq_msg);

bool
posix_mq_receive_msg(POSIX_MQ_CB *posix_mq_cb, POSIX_MQ_MSG *posix_mq_msg);

int posix_mq_get_errcode(POSIX_MQ_CB *posix_mq_cb);



#ifdef __cplusplus
}
#endif

#endif //POSIX_NQ_POSIXMQ_H
