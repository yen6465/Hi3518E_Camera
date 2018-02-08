/****************************************************************************************
* Copyright (c) 2012,ShenZhen NeedTek Electronic Ltd Co.                                 * 
* All rights reserved.                                                                  *
* **************************************************************************************/

#include "debuginfo.h"
#include <syslog.h>

static int debug_level = LEVEL_ERROR;
static int debug_target = TARGET_STDERR;
static char s_szLogBuffer[1024];

int DebugSetLevel( int level )
{
	int ret = debug_level;
	debug_level = level;
	return ret;
}

int DebugGetLevel( )
{
	return debug_level;
}

int DebugSetTarget( int target )
{
	int ret = debug_target;
	debug_target = target;
	return ret;
}

int DebugGetTarget()
{
	return debug_target;
}

void DebugSetDevelopMode(int sig)
{
    DebugSetLevel(LEVEL_DEBUG);
    DebugSetTarget(TARGET_SYSLOG);
}

void DebugPrintf(int level, const char *file, const char *func, int line, const char *fmt, ...)
{	
	if( level <= debug_level )
	{
        if( debug_target == TARGET_STDERR || debug_target == TARGET_SYSLOG )
        {
            va_list args;
            struct timeval tv;
            gettimeofday(&tv, NULL);

			va_start (args, fmt);
			vsnprintf(s_szLogBuffer,sizeof(s_szLogBuffer),fmt, args);
	    	va_end (args);

            if( debug_target == TARGET_SYSLOG )
            {
                syslog(LOG_DEBUG|LOG_USER,"[%ld.%03ld] %s",tv.tv_sec, tv.tv_usec / 1000,s_szLogBuffer);
            }
            else
            {
                printf("[%ld.%03ld] %s\n", tv.tv_sec, tv.tv_usec / 1000,s_szLogBuffer);
            }
        }
        else
        {
            return;
        }
	}
}

int DebugFramerate( unsigned long *frames, unsigned long *ticks, int len, const char *fmt, ...)
{
	va_list args;
	float framerate;
	unsigned long nowticks;
	struct timeval tv;
	if(debug_level >= LEVEL_DEBUG)
	{
		*frames = *frames + len;
		gettimeofday(&tv, NULL);
		nowticks = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
		if ( (nowticks - *ticks) >= (15 * 1000) )
		{
			framerate = (*frames * 1000.0) / (nowticks - *ticks);
			*ticks = nowticks;
			*frames = 0;
			va_start (args, fmt);
			vprintf (fmt, args);
			va_end (args);
			return printf("%.2f\n", framerate);
		}
	}
	return 0;
}


