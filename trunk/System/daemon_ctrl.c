#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_APPS_WDT
#ifdef CONFIG_APPS_WDT

#define COMMAND_SIZE 256
#define CONSOLE_NAME_SIZE 32
#define MAXENV	16		/* Number of env. vars */

/*进程监控功能的相关宏定义*/
#define PS_COMMAND "ps -a"  
#define SCAN(buf) (sscanf(buf,"%d %s %s %s",pid,s[0],s[1],cmd) == 4) 
#define UPGPROC_NUM 4               /*升级相关进程的数目*/
#define KEYPROC_NUM 3               /*待监控进程的数目*/
#define PSLINE_LENGTH 256           /*PS命令一行显示的最大字符数*/
#define PROCNAME_LENGTH 30          /*进程名称的最大字符数*/
#define PIDNUMBER 1                 /*需要查找的正在运行进程个数(同一进程有多个pid的情况)*/
#define PS_ASSISTITEM_NUM 3         /*PS命令中除了pid与进程名之外其他属性的个数*/
#define PS_ASSISTITEM_LENGTH 32     /*PS命令中除了pid与进程名之外其他属性内容最大字符数*/
static int proc_monitor(char *process);

static int lookup_pid(char *name, pid_t *pids, int n)  
{  
    FILE *fp;  
    char buf[PSLINE_LENGTH], cmd[PROCNAME_LENGTH], s[PS_ASSISTITEM_NUM][PS_ASSISTITEM_LENGTH];  
    pid_t *pid;  
   
    if ((fp = popen(PS_COMMAND, "r")) == NULL)  
        return -1;
   
    pid = pids;  
    while (n > 0 && fgets(buf, sizeof(buf), fp))  
        if (SCAN(buf) && strstr(cmd, name))
            pid++, n--;  
   
    pclose(fp);  
    return pid - pids;  
}  

static int proc_monitor(char *process)  
{  
    pid_t pids[PIDNUMBER];  
    int n, i = 0;  
   
    if ((n = lookup_pid(process, pids, PIDNUMBER)) == -1) {  
        printf("Error reading pid of %s %d!",process, pids[i]);
        return -1;  
    }  
	
    return n;  
}

int main(int argc, char* argv[])
{
	int procnum,procexist;

	/*关键进程数组定义，
	  扩展或裁减的时候只需要修改此处的具体进程名称，并修改宏定义即可*/
	char key_proc[KEYPROC_NUM][PROCNAME_LENGTH]={"boa" ,"zero_cmd_ether", "rpc_stream"};

	while(1)
	{
		procexist = 0;
		for(procnum=0;procnum < KEYPROC_NUM;procnum++)
		{
			procexist = proc_monitor(key_proc[procnum]);
			if(procexist < 1 )
			{
				//sleep(5);
				if(procnum == 0)
					system("/usr/sbin/boa&");
				else if(procnum == 1)
					system("/usr/bin/zero_cmd_ether&");	
				else if(procnum == 2)
					system("/usr/bin/rpc_stream&");	
			}					
		}
		sleep(1000 * 3);
	}


	return 0;
}
#endif


