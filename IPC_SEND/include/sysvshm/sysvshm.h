//
// Created by kervias on 20-8-12.
//

#ifndef SYSV_SHM_SYSVSHM_H
#define SYSV_SHM_SYSVSHM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/model.h"


bool
init_sysv_shm_cb(
        SYSV_SHM_CB *sysv_shm_cb,
        IPC_SEND_OR_RECEIVE type,
        char *path,
        size_t len,
        char *path_sem_wr,
        size_t len_sem_wr,
        char *path_sem_rd,
        size_t len_sem_rd
);

bool
close_sysv_shm_cb(SYSV_SHM_CB *sysv_shm_cb);

bool
sysv_shm_send_msg(SYSV_SHM_CB *sysv_shm_cb, SYSV_SHM_MSG *sysv_shm_msg);

bool
sysv_shm_receive_msg(SYSV_SHM_CB *sysv_shm_cb, SYSV_SHM_MSG *sysv_shm_msg);

int sysv_shm_get_errcode(SYSV_SHM_CB *sysv_shm_cb);


#ifdef __cplusplus
}
#endif


#endif //SYSV_SHM_SYSVSHM_H
