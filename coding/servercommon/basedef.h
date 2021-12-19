#ifndef __BASEDEF_H__
#define __BASEDEF_H__

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

#define TO_STRING(param) #param
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
