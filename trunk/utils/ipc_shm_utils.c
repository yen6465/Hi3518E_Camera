#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include "ipc_shm_utils.h"

ipc_shm_handle* ipc_shm_open(int key, int size, int flg)
{
	int shmid = shmget((key_t)key, size, 0666|IPC_CREAT);  
    if(shmid == -1)  
    {  
        fprintf(stderr, "shmget key:%d failed\n", key);  
        return NULL;
    }  

    char* shmaddr = (char*)shmat(shmid, NULL, 0);
	if((int)shmaddr == -1)
	{
		fprintf(stderr, "shmat shmid:%d failed\n", shmid);  
		return NULL;
	}

	ipc_shm_handle* handle = (ipc_shm_handle*)malloc(sizeof(ipc_shm_handle));
	memset(handle, 0x0, sizeof(ipc_shm_handle));
	
	handle->key = key;
	handle->id = shmid;
	handle->base = shmaddr;
	handle->size = size;
	handle->offset = 0;
	
	return handle;
}

void ipc_shm_close(ipc_shm_handle* handle, int flag)
{
    if(shmdt(handle->base) == -1)  
    {  
        fprintf(stderr, "shmdt id:%d failed\n", handle->id);  
        return;
    } 

	if(flag != 0)
	{
	    if(shmctl(handle->id, IPC_RMID, 0) == -1)  
	    {  
	        fprintf(stderr, "shmctl(IPC_RMID) id:%d failed\n", handle->id);  
	        return;
	    } 
	}
		
	if(handle != NULL)
	{
		free(handle);
	}
}

int ipc_shm_write(ipc_shm_handle* handle, char* data, int length)
{
	if(handle == NULL || handle->base == NULL || data == NULL)
	{
		return -1;
	}

	if(handle->offset + length > handle->size)
	{
		memcpy(handle->base, data, length);
		handle->offset = length;
	}
	else
	{
		memcpy(handle->base + handle->offset, data, length);
		handle->offset += length;
	}

	return 0;
}

int ipc_shm_read(ipc_shm_handle* handle, char* data, int lenght, int offset)
{
	if(handle == NULL || handle->base == NULL || data == NULL)
		return -1;
	if(offset+lenght > handle->size)
		return -2;

	memcpy(data, handle->base + offset, lenght);
	
	return 0;
}
