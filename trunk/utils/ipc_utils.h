#ifndef IPC_UTILS
#define IPC_UTILS

#include <pthread.h>

#define BROADCAST		0
#define PASS_THROUGH	0

#define MAX_CLNT_LEN 	20
#define MAX_MSG_LEN 	1024

typedef struct _ipc_st
{
	int  type;
	int  form;
	int  to;
	char body[MAX_MSG_LEN];
}ipc_st;

typedef struct _clnt_msg_info
{
	int key;
	int id;
}ipc_clnt_info;

typedef struct ipc_srv_handle_s
{
	int				key;
	int 			id;
	int 			clnt_count;
	ipc_clnt_info 	clnt_info[MAX_CLNT_LEN];
	int				tid_stop;
	pthread_t 		tid;
	int (*on_conn_recv)(struct ipc_srv_handle_s* srv, ipc_st data);
}ipc_srv_handle;

typedef struct ipc_clnt_handle_s
{
	int key;
	int srv_key;
	int id;
	int srv_id;
	int			tid_stop;
	pthread_t 	tid;
	int (*on_conn_recv)(struct ipc_clnt_handle_s* clnt, ipc_st data);
}ipc_clnt_handle;

ipc_srv_handle* ipc_srv_create(int key, int (*on_conn_recv)(struct ipc_srv_handle_s* srv, ipc_st data));
void ipc_srv_destory(ipc_srv_handle* srv);
int ipc_srv_clnt_login(ipc_srv_handle* srv, int clnt_key);
int ipc_srv_clnt_logout(ipc_srv_handle* srv, int clnt_key, int del);
int ipc_srv_send(ipc_srv_handle* srv, int to_id, char* data, int length, int type);

ipc_clnt_handle* ipc_clnt_create(int key, int (*on_conn_recv)(struct ipc_clnt_handle_s* clnt, ipc_st data));
void ipc_clnt_destory(ipc_clnt_handle* clnt);
int ipc_clnt_login(ipc_clnt_handle* clnt, int srv_key);
int ipc_clnt_logout(ipc_clnt_handle* clnt);
int ipc_clnt_send(ipc_clnt_handle* clnt, char* data, int length, int type, int to);

#endif
