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

static void* thread_ipc_clnt_send(void* param)
{
	ipc_clnt_handle* handle = (ipc_clnt_handle*)param;

	int count = 0;
	//向消息队列中写消息，直到写入end
	while(1)
	{
		char buffer[128] = {0};
		sprintf(buffer, "clnt1 %d", count);
		if(ipc_clnt_send(handle, buffer, strlen(buffer)+1, 1, MSG_RECV2) == -1)
		{
			LOGE_print("msgsnd failed %d", errno);
			continue;
		}
		count++;
		usleep(2*1000*1000);
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
	log_ctrl* log = log_ctrl_create("rpc_utils_clnt.log", LOG_DEBUG, 1);

	int ret;
	ipc_clnt_handle* handle = ipc_clnt_create(MSG_RECV1, on_conn_recv);
	if(handle == NULL)
		return -1;

	ret = ipc_clnt_login(handle, MSG_SERVER);
	if(ret != 0)
	{
		return -1;
	}

	pthread_t tid;
	if(pthread_create(&tid, NULL, thread_ipc_clnt_send, handle) != 0)
	{
		LOGE_print("create thread thread_ipc_clnt_send error %d", errno);
		return -1;
	}

	while(1)
	{
		usleep(20*1000);
	}
	
	ipc_clnt_destory(handle);
	log_ctrl_destory(log);

	return 0;
}
