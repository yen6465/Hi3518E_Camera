#ifndef IPC_SHM_UTILS_H
#define IPC_SHM_UTILS_H

#include <sys/shm.h>

typedef struct ipc_shm_handle_s
{
	int key;
	int id;
	char* base;
	size_t size;
	int offset;
}ipc_shm_handle;

ipc_shm_handle* ipc_shm_open(int key, int size, int flg);
void ipc_shm_close(ipc_shm_handle* handle, int flag);

int ipc_shm_write(ipc_shm_handle* handle, char* data, int length);
int ipc_shm_read(ipc_shm_handle* handle, char* data, int lenght, int offset);


#endif
