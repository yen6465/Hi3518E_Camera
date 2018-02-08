#include <stdio.h>  
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "ipc_utils.h"
#include "ipc_shm_utils.h"
#include "utils_log.h"

#define SHM_KEY				131415
#define SHM_DEFAULT_SIZE	1024*1024*2

#define BROADCAST		0
#define MSG_SERVER	1234
#define MSG_RECV1	12341
#define MSG_RECV2	12342
#define MSG_SEND1	12343

static int on_conn_recv(struct ipc_clnt_handle_s* clnt, ipc_st data)
{
//	LOGD_print("msgrcv data data.body:%s data.type:%d data.from:%d data.to:%d", data.body, data.type, data.form, data.to);
	return 0;
}

typedef struct shm_node_s
{
	int offset;
	int length;
}shm_node_s;

int main(int argc, char* argv[])
{
	log_ctrl* log = log_ctrl_create("shm_utils_write.log", LOG_DEBUG, 1);
	
	ipc_shm_handle* handle = ipc_shm_open(SHM_KEY, SHM_DEFAULT_SIZE, 0);
	ipc_clnt_handle* clnt = ipc_clnt_create(MSG_SEND1, on_conn_recv);
	ipc_clnt_login(clnt, MSG_SERVER);
	
	int count = 0;
	while(1)
	{
		shm_node_s node;
		node.offset = handle->offset;
		
		char str[1024] = {0};
		sprintf(str,"shm_utils_write count:%d", count);
		
		int ret = ipc_shm_write(handle, str, strlen(str));
		if(ret == 0)
		{
			node.length = strlen(str);
			ipc_clnt_send(clnt, (char*)&node, sizeof(shm_node_s), 1, BROADCAST);
		}
		count++;
		usleep(2*1000*1000);
	}

	ipc_clnt_logout(clnt);
	ipc_clnt_destory(clnt);
	ipc_shm_close(handle, 0);
	log_ctrl_destory(log);
	
	return 0;
}