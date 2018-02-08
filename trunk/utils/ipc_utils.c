#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/msg.h> 

#include "ipc_utils.h"
#include "utils_log.h"

static void* ipc_srv_recv_thread(void* param);

ipc_srv_handle* ipc_srv_create(int key, int (*on_conn_recv)(struct ipc_srv_handle_s* srv, ipc_st data))
{
	int msgid = -1;
	int i = 0;
	
	msgid = msgget((key_t)key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		LOGE_print("msgget failed with error: %d", errno);
		return NULL;
	}

	LOGI_print("key:%d, id:%d", key, msgid);
	ipc_srv_handle* handle = (ipc_srv_handle*)malloc(sizeof(ipc_srv_handle));

	handle->key = key;
	handle->id = msgid;
	handle->clnt_count = 0;
	memset(handle->clnt_info, 0, sizeof(handle->clnt_info));
	handle->tid_stop = 0;
	handle->on_conn_recv = on_conn_recv;

	if(pthread_create(&handle->tid, NULL, ipc_srv_recv_thread, handle) != 0)
	{
		LOGE_print("pthread_create ipc_srv_thread_forwarding error:%s", strerror(errno));
		ipc_srv_destory(handle);
		return NULL;
	}
		
	return handle;
}

void ipc_srv_destory(ipc_srv_handle* srv)
{
	if(srv->tid != 0)
	{
		srv->tid_stop == 1;
		pthread_join(srv->tid, NULL);
		srv->tid = 0;
	}
		
	if(srv != NULL)
	{
		free(srv);
	}
}

void* ipc_srv_recv_thread(void* param)
{
	ipc_srv_handle* srv = (ipc_srv_handle*)param;
	LOGI_print("key:%d, id:%d", srv->key, srv->id);
	while(srv->tid_stop != 1)
	{
		ipc_st msg;
		if(msgrcv(srv->id, (void*)&msg, sizeof(ipc_st), 0, 0) == -1)
		{
			LOGE_print("msgrcv failed with errno: %d", errno);
			continue;
		}

		LOGD_print("msgrcv msg.type:%d msg.from:%d msg.to:%d", msg.type, msg.form, msg.to);

		if(srv->on_conn_recv != NULL)
		{
			int ret = (*srv->on_conn_recv)(srv, msg);
			if(ret != 0 )
			{
				LOGW_print("on_conn_recv error %d", errno);
			}
		}
	}

	return NULL;
}

int ipc_srv_clnt_login(ipc_srv_handle* srv, int clnt_key)
{
	LOGI_print("clnt_key:%d", clnt_key);
	int i = 0;
	if(srv->clnt_count >= MAX_CLNT_LEN)
	{
		LOGE_print("rpc_clnt_register error, full...");
		return -1;
	}

	int msgid = -1;
	msgid = msgget((key_t)clnt_key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		LOGE_print("msgget failed with error: %d", errno);
		return -1;
	}
	
	//查找空位
	for(i; i < MAX_CLNT_LEN; i++)
	{
		if(srv->clnt_info[i].id == 0 || srv->clnt_info[i].id == msgid)
		{
			break;
		}
	}

	if(i == MAX_CLNT_LEN)
	{
		LOGE_print("rpc_clnt_register error, full...");
		return -1;
	}

	srv->clnt_info[i].id = msgid;
	srv->clnt_info[i].key = clnt_key;
	srv->clnt_count++;

	i = 0;
	for(i; i < MAX_CLNT_LEN; i++)
	{
		fprintf(stderr, "%d ",srv->clnt_info[i].id);
	}
	fprintf(stderr, "\n");

	return 0;
}

int ipc_srv_clnt_logout(ipc_srv_handle* srv, int clnt_key, int del)
{
	int msgid = 0;
	int i = 0;

	//查找空位
	for(i; i < MAX_CLNT_LEN; i++)
	{
		if(srv->clnt_info[i].key == clnt_key)
		{
			srv->clnt_count--;
			break;
		}
	}
	msgid = srv->clnt_info[i].id;
	srv->clnt_info[i].id = 0;
	srv->clnt_info[i].key = 0;
	
	if(del == 1 && msgid != 0)
	{
		//删除消息队列
		if(msgctl(msgid, IPC_RMID, 0) == -1)
		{
			LOGE_print("msgctl(IPC_RMID) failed %d", errno);
			//return -1;
		}
	}

	i = 0;
	for(i; i < MAX_CLNT_LEN; i++)
	{
		fprintf(stderr, "%d ",srv->clnt_info[i].id);
	}
	fprintf(stderr, "\n");
	LOGI_print("msgid:%d unregister", msgid);

	return 0;

}

int ipc_srv_send(ipc_srv_handle* srv, int to_id, char* data, int length, int type)
{
	//向队列发送数据
	if(type == PASS_THROUGH)
	{
		if(msgsnd(to_id, (void*)data, length, IPC_NOWAIT) == -1)
		{
			LOGE_print("msgsnd failed %d", errno);
			return -1;
		}
	}
	else
	{

	}

	return 0;
}

static void* ipc_clnt_recv_thread(void* param);

ipc_clnt_handle* ipc_clnt_create(int key, int (*on_conn_recv)(struct ipc_clnt_handle_s* clnt, ipc_st data))
{
	int clntid = -1;
	
	clntid = msgget((key_t)key, 0666 | IPC_CREAT);
	if(clntid == -1)
	{
		LOGE_print("msgget clnt_key:%d failed with error: %d", key, errno);
		return NULL;
	}
	LOGI_print("msgget clnt_key:%d clntid:%d", key, clntid);

	ipc_clnt_handle* handle = (ipc_clnt_handle*)malloc(sizeof(ipc_clnt_handle));
	
	handle->key = key;
	handle->id  = clntid;
	handle->on_conn_recv = on_conn_recv;

	handle->tid_stop = 0;
	if(pthread_create(&handle->tid, NULL, ipc_clnt_recv_thread, handle) != 0)
	{
		LOGE_print("pthread_create ipc_srv_thread_forwarding error:%s", strerror(errno));
		ipc_clnt_destory(handle);
		return NULL;
	}

	return handle;
}

void ipc_clnt_destory(ipc_clnt_handle* clnt)
{
	if(clnt->tid != 0)
	{
		clnt->tid_stop == 1;
		pthread_join(clnt->tid, NULL);
		clnt->tid = 0;
	}
		
	if(clnt != NULL)
	{
		free(clnt);
	}
}

void* ipc_clnt_recv_thread(void* param)
{
	ipc_clnt_handle* handle = (ipc_clnt_handle*)param;
	while(handle->tid_stop != 1)
	{
		ipc_st data;
		if(msgrcv(handle->id, (void*)&data, sizeof(ipc_st), 0, 0) == -1)
		{
			LOGE_print("msgrcv failed with errno: %d", errno);
			continue;
		}

//		LOGD_print("msgrcv data data.body:%s data.type:%d data.from:%d data.to:%d", data.body, data.type, data.form, data.to);

		if(handle->on_conn_recv != NULL)
		{
			(*handle->on_conn_recv)(handle, data);
		}
	}
	
	return NULL;
}


int ipc_clnt_login(ipc_clnt_handle* clnt, int srv_key)
{
	int srvid = -1;
	srvid = msgget((key_t)srv_key, 0666 | IPC_CREAT);
	if(srvid == -1)
	{
		LOGE_print("msgget srv_key:%d failed with error: %d", srv_key, errno);
		return -1;
	}
	LOGI_print("msgget svc_key:%d svcid:%d\n", srv_key, srvid);

	ipc_st msg;
	msg.type = 0;
	msg.form = clnt->key;
	msg.to = srv_key;
	
	char str[25];
    sprintf(str, "%d", clnt->key);
	
	memcpy(msg.body, str, sizeof(str));
	if(msgsnd(srvid, (void*)&msg, sizeof(ipc_st), IPC_NOWAIT) == -1)
	{
		LOGE_print("msgsnd failed %d", errno);
		return -1;
	}

	clnt->srv_id = srvid;
	clnt->srv_key = srv_key;
	
	return 0;
}

int ipc_clnt_logout(ipc_clnt_handle* clnt)
{
	ipc_st msg;
	msg.type = -1;
	msg.form = clnt->key;
	msg.to = clnt->srv_key;
	
	char str[25];
    sprintf(str, "%d", clnt->key);
	
	memcpy(msg.body, str, sizeof(str));
	if(msgsnd(clnt->srv_id, (void*)&msg, sizeof(ipc_st), IPC_NOWAIT) == -1)
	{
		LOGE_print("msgsnd failed %d", errno);
		return -1;
	}

	return 0;
}

int ipc_clnt_send(ipc_clnt_handle* clnt, char* data, int length, int type, int to)
{
	if(data == NULL || length > MAX_MSG_LEN) 
		return -1;
	
	ipc_st msg;
	msg.type = type;
	msg.form = clnt->key;
	msg.to = to;
	memcpy(msg.body, data, length);
	
	if(msgsnd(clnt->srv_id, (void*)&msg, sizeof(ipc_st), IPC_NOWAIT) == -1)
	{
		LOGE_print("msgsnd failed %d", errno);
		return -1;
	}

	return 0;
}


