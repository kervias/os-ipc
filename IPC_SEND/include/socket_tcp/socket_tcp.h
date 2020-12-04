//
// Created by kervias on 20-8-17.
//

#ifndef SOCKET_TCP_SOCKET_TCP_H
#define SOCKET_TCP_SOCKET_TCP_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../common/model.h"
bool
init_socket_tcp_cb(
    SOCKET_TCP_CB *socket_tcp_cb,
    IPC_SEND_OR_RECEIVE type,
    char *ip_send,
    size_t ip_send_len,
    unsigned short port_send,
    char *ip_receive,
    size_t ip_receive_len,
    unsigned short port_receive
);

bool close_socket_tcp_cb(SOCKET_TCP_CB *socket_tcp_cb);

bool socket_tcp_accept(SOCKET_TCP_CB *socket_tcp_cb);

bool socket_tcp_receive_msg(SOCKET_TCP_CB *socket_tcp_cb, SOCKET_TCP_MSG *socket_tcp_msg);
bool socket_tcp_send_msg(SOCKET_TCP_CB *socket_tcp_cb, SOCKET_TCP_MSG *socket_tcp_msg);

int socket_tcp_get_errcode(SOCKET_TCP_CB *socket_tcp_cb);

#ifdef __cplusplus
}
#endif

#endif //SOCKET_TCP_SOCKET_TCP_H
