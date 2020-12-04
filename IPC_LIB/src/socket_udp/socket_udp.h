//
// Created by kervias on 20-8-17.
//

#ifndef SOCKET_UDP_SOCKET_UDP_H
#define SOCKET_UDP_SOCKET_UDP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/model.h"

bool
init_socket_udp_cb(
        SOCKET_UDP_CB *socket_udp_cb,
        IPC_SEND_OR_RECEIVE type,
        char *ip_send,
        size_t ip_send_len,
        unsigned short port_send,
        char *ip_receive,
        size_t ip_receive_len,
        unsigned short port_receive
);

bool close_socket_udp_cb(SOCKET_UDP_CB *socket_udp_cb);

bool socket_udp_receive_msg(SOCKET_UDP_CB *socket_udp_cb, SOCKET_UDP_MSG *socket_udp_msg);

bool socket_udp_send_msg(SOCKET_UDP_CB *socket_udp_cb, SOCKET_UDP_MSG *socket_udp_msg);

int socket_udp_get_errcode(SOCKET_UDP_CB *socket_udp_cb);

#ifdef __cplusplus
}
#endif

#endif //SOCKET_UDP_SOCKET_UDP_H
