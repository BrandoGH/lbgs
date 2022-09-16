#include "minidump.h"

#ifdef LINUX_OS
#include <cstddef>
#endif // LINUX_OS

void* LbgsMinisDump::autoDump()
{
#ifdef WIN_OS
	return (void*)SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif // WIN_OS
	return NULL;
}

LbgsMinisDump::LbgsMinisDump()
{
}

LbgsMinisDump::~LbgsMinisDump()
{
}

#ifdef WIN_OS
LONG LbgsMinisDump::ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
	TCHAR szDumpDir[MAX_PATH] = {0};
	TCHAR szDumpFile[MAX_PATH] = {0};
	TCHAR szMsg[MAX_PATH] = {0};
	SYSTEMTIME stTime = {0};
	// buld dump file path
	GetLocalTime(&stTime);
	::GetCurrentDirectory(MAX_PATH, szDumpDir);
	wsprintf(szDumpFile, _T("%s\\%04d%02d%02d__%02d_%02d_%02d.dmp"), szDumpDir,
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);
	// create dump file
	CreateDumpFile(szDumpFile, pException);

	// create a error dialog tip, and exit process
	wsprintf(szMsg, _T(
		"LBGS The game server has problems, and the corresponding DUMP file has been generated.the path[%s]"
	), szDumpFile);
	FatalAppExit(-1, szMsg);

	return EXCEPTION_EXECUTE_HANDLER;
}

void LbgsMinisDump::CreateDumpFile(TCHAR* lpstrDumpFilePathName, EXCEPTION_POINTERS* pException)
{
	// create dump file
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// dump info
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	//  write info to dump file
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithFullMemory, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}
#endif // WIN_OS

