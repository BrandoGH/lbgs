#ifndef __BASEDEF_H__
#define __BASEDEF_H__

#include "basedef.h"

#include <spdlog/spdlog.h>
#include <memory>

#ifdef WIN_OS
#include <windows.h>
#elif LINUX_OS
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>  
#endif

// 心跳包发送回显字符
#define I_MSG_HEART_CS "\x4C\x42\x47\x53"
#define I_MSG_HEART_SC "\x53\x47\x42\x4C"

// printf函数输出颜色
#define PRINTF_COLOR_CLEAR "\033[0m"		//表示清除前面设置的格式
#define PRINTF_RED "\033[1;31;40m"		//40:背景色为黑色, 1:表示高亮
#define PRINTF_BLUE "\033[1;34;40m"
#define PRINTF_GREEN "\033[1;32;40m"
#define PRINTF_YELLOW "\033[1;33;40m"
// ##__VA_ARGS__ 宏前面加上##的作用在于，当可变参数的个数为0时，这里的##起到把前面多余的","去掉的作用，否则会编译出错
#define printf_color(color, str, ...) \
	printf(color str PRINTF_COLOR_CLEAR, ##__VA_ARGS__ )

#define TO_STRING(param) #param
#define DEFINE_BYTE_ARRAY(name,len)	\
	byte name[len];					\
	memset(name,0,len)

#define DEL_OBJ(obj)	\
	if(obj)				\
	{					\
		delete obj;		\
		obj = NULL;		\
	}

#define DEL_ARR(arr)	\
	if(arr)				\
	{					\
		delete[] arr;	\
		arr = NULL;		\
	}


#ifdef WIN_OS
#define THREAD_ID GetCurrentThreadId()
#define PROCESS_ID GetCurrentProcessId()
#elif LINUX_OS
#define THREAD_ID syscall(SYS_gettid)
#define PROCESS_ID getpid()
#endif

typedef unsigned char			uchar,	byte;
typedef unsigned short			ushort;
typedef unsigned int			uint;
typedef unsigned long			ulong;
typedef unsigned long long		ullong;

namespace Common
{
// log
typedef std::shared_ptr<spdlog::logger> LoggerPtr;
}

#endif // !__BASEDEF_H__
