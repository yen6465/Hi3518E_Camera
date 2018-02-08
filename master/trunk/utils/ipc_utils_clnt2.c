#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>
#include <pthread.h>

#include "ipc_utils.h"
#include "utils_log.h"

#define MSG_SERVER	1234
#define MSG_RECV1	12341
#define MSG_RECV2	12342

static void* thread_msg_revc(void* param)
{
	int clntid = *((int*)param);

	ipc_st msg;
	//向消息队列中写消息，直到写入end
	while(1)
	{
		if(msgrcv(clntid, (void*)&msg, sizeof(ipc_st), 0, 0) == -1)
		{
			LOGE_print("msgrcv failed with errno: %d", errno);
			continue;
		}

		LOGD_print("msgrcv msg smg.body:%s msg.type:%d msg.from:%d msg.to:%d", msg.body, msg.type, msg.form, msg.to);
	}
	return NULL;
}

static int on_conn_recv(struct ipc_clnt_handle_s* clnt, ipc_st data)
{
	LOGD_print("msgrcv data data.body:%s data.type:%d data.from:%d data.to:%d", data.body, data.type, data.form, data.to);
	return 0;
}

int main()
{
	log_ctrl* log = log_ctrl_create("rpc_utils_clnt2.log", LOG_DEBUG, 1);
	
	int ret;
	ipc_clnt_handle* handle = ipc_clnt_create(MSG_RECV2, on_conn_recv);
	if(handle == NULL)
		return -1;

	ret = ipc_clnt_login(handle, MSG_SERVER);
	if(ret != 0)
	{
		return -1;
	}

	int count = 0;
	while(1)
	{
		char buffer[128] = {0};
		sprintf(buffer, "clnt2 %d", count);
		if(ipc_clnt_send(handle, buffer, strlen(buffer)+1, 1, MSG_RECV1) == -1)
		{
			LOGE_print("msgsnd failed %d", errno);
			continue;
		}
		count++;
		usleep(1*1000*1000);
	}
	
	ipc_clnt_destory(handle);
	log_ctrl_destory(log);

	return 0;
}