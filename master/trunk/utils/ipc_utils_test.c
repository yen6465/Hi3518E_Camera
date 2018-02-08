#include <stdio.h>  
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "ipc_utils.h"
#include "utils_log.h"

#define MSG_SERVER	1234

static int on_conn_recv(ipc_srv_handle* srv, ipc_st msg)
{
	int i = 0;
	
	switch(msg.type)
	{
	case -1:
		ipc_srv_clnt_logout(srv,atoi(msg.body),1);
		break;
	case 0:
		ipc_srv_clnt_login(srv,atoi(msg.body));
		break;
	case 1:
		if(msg.to == BROADCAST)
		{
			for(i; i < MAX_CLNT_LEN; i++)
			{
				if(srv->clnt_info[i].key != 0 && srv->clnt_info[i].key != msg.form)
				{
					//向队列发送数据
					if(ipc_srv_send(srv, srv->clnt_info[i].id, (void*)&msg, sizeof(ipc_st), PASS_THROUGH) == -1)
					{
						LOGE_print("msgsnd failed %d", errno);
						ipc_srv_clnt_logout(srv, srv->clnt_info[i].key, 1);
						return -1;
					}
				}
			}
		}
		else
		{
			for(i; i < MAX_CLNT_LEN; i++)
			{
				if(srv->clnt_info[i].key == msg.to)
				{
					if(ipc_srv_send(srv, srv->clnt_info[i].id, (void*)&msg, sizeof(ipc_st), PASS_THROUGH) == -1)
					{
						LOGE_print("msgsnd failed %d", errno);
						ipc_srv_clnt_logout(srv, srv->clnt_info[i].key, 1);
						return -1;
					}
				}
			}
		}
		break;
	default:
		
		break;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int ret = 0;
	ipc_st msg;

	log_ctrl* log = log_ctrl_create("rpc_utils_test.log", LOG_TRACE, 1);

	ipc_srv_handle* srv = ipc_srv_create(MSG_SERVER, on_conn_recv);

	while(1)
	{
		usleep(20*1000);
	}
	
	ipc_srv_destory(srv);
	log_ctrl_destory(log);
	
	return 0;
}
