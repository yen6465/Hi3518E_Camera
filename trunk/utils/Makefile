all: ipc_utils_test shm_utils_write shm_utils_read1 shm_utils_read2
#ipc_utils_clnt ipc_utils_clnt2 
ipc_utils_clnt:
	gcc ipc_utils_clnt.c ipc_utils.c utils_log.c -o ipc_utils_clnt -lpthread

ipc_utils_clnt2:
	gcc ipc_utils_clnt2.c ipc_utils.c utils_log.c -o ipc_utils_clnt2 -lpthread
	
ipc_utils_test:
	gcc ipc_utils_test.c ipc_utils.c utils_log.c -o ipc_utils_test -lpthread

shm_utils_write:
	gcc shm_utils_write.c ipc_utils.c ipc_shm_utils.c utils_log.c -o shm_utils_write -lpthread

shm_utils_read1:
	gcc shm_utils_read1.c ipc_utils.c ipc_shm_utils.c utils_log.c -o shm_utils_read1 -lpthread

shm_utils_read2:
	gcc shm_utils_read2.c ipc_utils.c ipc_shm_utils.c utils_log.c -o shm_utils_read2 -lpthread
	
clean:
	#rm ipc_utils_clnt
	#rm ipc_utils_clnt2
	rm ipc_utils_test
	rm shm_utils_write
	rm shm_utils_read1
	rm shm_utils_read2