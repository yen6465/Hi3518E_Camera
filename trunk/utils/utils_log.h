#ifndef   _UTILS_SYSLOG_H_
#define   _UTILS_SYSLOG_H_

#include <stdio.h>
#include <stdlib.h>

//宏定义
#define MAX_LOG_BUFSIZE		2048
#define MAX_LOG_FILESIZE	100*1024


#define   LOG_EMERG   0
#define   LOG_ERR     1
#define   LOG_WARN    2
#define   LOG_INFO    3
#define   LOG_DEBUG   4
#define   LOG_TRACE   5

//打印字体颜色
#define NONE         "\033[m" 
#define RED          "\033[0;32;31m" 
#define LIGHT_RED    "\033[1;31m" 
#define GREEN        "\033[0;32;32m" 
#define LIGHT_GREEN  "\033[1;32m" 
#define BLUE         "\033[0;32;34m" 
#define LIGHT_BLUE   "\033[1;34m" 
#define DARY_GRAY    "\033[1;30m" 
#define CYAN         "\033[0;36m" 
#define LIGHT_CYAN   "\033[1;36m" 
#define PURPLE       "\033[0;35m" 
#define LIGHT_PURPLE "\033[1;35m" 
#define BROWN        "\033[0;33m" 
#define YELLOW       "\033[1;33m" 
#define LIGHT_GRAY   "\033[0;37m" 
#define WHITE        "\033[1;37m"

typedef struct log_ctrl_s
{
	FILE*	fd;
	char	file[128];
	int		level;
	int		wt;
}log_ctrl; 

log_ctrl* log_ctrl_create(char* file, int level, int wt);
void log_ctrl_destory(log_ctrl* log);
int  log_ctrl_level_set(log_ctrl* log, int level);
int  log_ctrl_wt_set(log_ctrl* log,int wt);
int  log_ctrl_file_write(log_ctrl* log, char* data, int len);
int  log_ctrl_print(log_ctrl* log, int level, char* t, ...);

#define LOGT_print(t, ...)	log_ctrl_print(NULL, LOG_TRACE, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGD_print(t, ...) 	log_ctrl_print(NULL, LOG_DEBUG, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGI_print(t, ...) 	log_ctrl_print(NULL, LOG_INFO,  "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGW_print(t, ...)	log_ctrl_print(NULL, LOG_WARN,  "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGE_print(t, ...) 	log_ctrl_print(NULL, LOG_ERR,   "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGM_print(t, ...) 	log_ctrl_print(NULL, LOG_EMERG, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define CLOGT_print(c, t, ...)	log_ctrl_print(c, LOG_TRACE, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CLOGD_print(c, t, ...) 	log_ctrl_print(c, LOG_DEBUG, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CLOGI_print(c, t, ...) 	log_ctrl_print(c, LOG_INFO,  "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CLOGW_print(c, t, ...)	log_ctrl_print(c, LOG_WARN,  "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CLOGE_print(c, t, ...) 	log_ctrl_print(c, LOG_ERR,   "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CLOGM_print(c, t, ...) 	log_ctrl_print(c, LOG_EMERG, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif

