//
// Created by kervias on 20-8-12.
//

#ifndef POSIXSHM_POSIXSHM_H
#define POSIXSHM_POSIXSHM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/model.h"


bool
init_posix_shm_cb(
        POSIX_SHM_CB *posix_shm_cb,
        IPC_SEND_OR_RECEIVE type,
        char *path,
        size_t len,
        char *path_sem_wr,
        size_t len_sem_wr,
        char *path_sem_rd,
        size_t len_sem_rd
        );

bool
close_posix_shm_cb(POSIX_SHM_CB *posix_shm_cb);

bool
posix_shm_send_msg(POSIX_SHM_CB *posix_shm_cb, POSIX_SHM_MSG *posix_shm_msg);

bool
posix_shm_receive_msg(POSIX_SHM_CB *posix_shm_cb, POSIX_SHM_MSG *posix_shm_msg);

int posix_shm_get_errcode(POSIX_SHM_CB *posix_shm_cb);


#ifdef __cplusplus
}
#endif
#endif //POSIXSHM_POSIXSHM_H
