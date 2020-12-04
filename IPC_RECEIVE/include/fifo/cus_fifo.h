//
// Created by kervias on 20-8-11.
//

#ifndef FIFO_CUS_FIFO_H
#define FIFO_CUS_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif


#include "../common/model.h"



bool init_fifo_cb(
    FIFO_CB *fifo_cb, //FIFO
    IPC_SEND_OR_RECEIVE type,
    char *fifo_path, //FIFO路径
    size_t len //FIFO路径长度
);

bool
close_fifo_cb(FIFO_CB *fifo_cb);

bool
fifo_send_msg(FIFO_CB *fifo_cb, FIFO_MSG *fifo_msg);

bool
fifo_receive_msg(FIFO_CB *fifo_cb, FIFO_MSG *fifo_msg);

int fifo_get_errcode(FIFO_CB *fifo_cb);

size_t fifo_get_pipe_buf();
//size_t fifo_get_pipe_size();

#ifdef __cplusplus
}
#endif

#endif //FIFO_CUS_FIFO_H
