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

// heart msg content
#define I_MSG_HEART_CS "\x4C\x42\x47\x53"
#define I_MSG_HEART_SC "\x53\x47\x42\x4C"

// printf function couput color
#define PRINTF_COLOR_CLEAR "\033[0m"		// clear previously all color
#define PRINTF_RED "\033[1;31;40m"			//40:background color black, 1:highlight
#define PRINTF_BLUE "\033[1;34;40m"
#define PRINTF_GREEN "\033[1;32;40m"
#define PRINTF_YELLOW "\033[1;33;40m"
/*
* ##__VA_ARGS__
*	The function of adding ## in front of the __VA_ARGS__ macro is that when the number of variable parameters is 0, 
*	the ## here plays the role of removing the redundant "," in front, otherwise it will compile an error
*/
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
typedef char					CString32[32];
typedef char					CString64[64];
typedef char					CString128[128];
typedef char					CString512[512];

union MsgFloat
{
	MsgFloat()
	{
		reset();
	}
	MsgFloat(float inFloat)
	{
		m_float = inFloat;
	}
	MsgFloat& operator=(float inFloat)
	{
		m_float = inFloat;
		return *this;
	}
	void reset()
	{
		m_float = 0.f;
	}

	float m_float;
	byte m_bytes[sizeof(float)];
};

union MsgDouble
{
	MsgDouble()
	{
		reset();
	}
	MsgDouble(double inDouble)
	{
		m_double = inDouble;
	}
	MsgDouble& operator=(double inDouble)
	{
		m_double = inDouble;
		return *this;
	}
	void reset()
	{
		m_double = 0.0;
	}

	double m_double;
	byte m_bytes[sizeof(double)];
};

namespace Common
{
// log
typedef std::shared_ptr<spdlog::logger> LoggerPtr;
}

#endif // !__BASEDEF_H__
