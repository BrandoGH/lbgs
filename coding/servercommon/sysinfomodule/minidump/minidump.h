#ifndef __MINIDUMP_H__
#define __MINIDUMP_H__

#ifdef WIN_OS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <DbgHelp.h>
#endif
class LbgsMinisDump
{
public:
	static void* autoDump();

private:
	LbgsMinisDump();
	~LbgsMinisDump();

#ifdef WIN_OS
	static LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException);
	static void CreateDumpFile(TCHAR* lpstrDumpFilePathName, EXCEPTION_POINTERS* pException);
#endif // WIN_OS
};




#endif
