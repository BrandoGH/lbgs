#include "sysinfo.h"
#include "logmodule/logdef.h"

#ifdef WIN_OS
#include <windows.h>
#include <tlhelp32.h>
#include <comdef.h>
#endif 

#ifdef LINUX_OS
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#endif 

namespace SystemInfoNS
{
	std::string g_strCurProcessName;
}

SystemInfo::SystemInfo()
{
}

SystemInfo::~SystemInfo()
{
}

bool SystemInfo::isProcessRuning(const std::string& processName)
{
#ifdef WIN_OS
	bool ret = false;
	HANDLE info_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //拍摄系统中所有进程的快照
	if(info_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32W program_info;
	program_info.dwSize = sizeof(PROCESSENTRY32W);  //设置结构体大小
	int bResult = Process32FirstW(info_handle, &program_info); //获取所有进程中第一个进程的信息
	if(!bResult)
	{
		return false;
	}

	while(bResult)
	{
		std::string pro_name = _bstr_t(program_info.szExeFile);
		if(processName == pro_name)
		{
			ret = true;
			break;
		}
		//获得下一个进程的进程信息
		bResult = Process32NextW(info_handle,&program_info);
	}
	CloseHandle(info_handle);//关闭句柄
	return ret;
#endif 

	// TODO 添加linux实现
	return false;
}

#ifdef WIN_OS
unsigned long double SystemInfo::getAvailableMemory(int mode)
{

	MEMORYSTATUSEX osSatex;
	osSatex.dwLength = sizeof(osSatex);
	GlobalMemoryStatusEx(&osSatex);
	unsigned long double availPhys = osSatex.ullAvailPhys;
	switch (mode)
	{
	case UNIT_B:
		break;

	case UNIT_K:
		availPhys = division(availPhys);
		break;

	case UNIT_M:
		availPhys = division(availPhys,2);
		break;

	case UNIT_G:
		availPhys = division(availPhys, 3);
		break;
	}

	return availPhys;
}
#elif LINUX_OS
ulong SystemInfo::getAvailableMemory(int mode)
{
	struct sysinfo info;
	int error = sysinfo(&info);
	if (error != 0)
	{
		LOG_SERVER_COMMON_CONFIG.printLog("error[%d], default return 0", error);
		return 0;
	}
	ulong availPhys = info.freeram;

	switch (mode)
	{
	case SystemInfo::UNIT_B:
		break;

	case SystemInfo::UNIT_K:
		availPhys = division(availPhys);
		break;

	case SystemInfo::UNIT_M:
		availPhys = division(availPhys, 2);
		break;

	case SystemInfo::UNIT_G:
		availPhys = division(availPhys, 3);
		break;
	}

	return availPhys;
}
#endif

#ifdef WIN_OS
unsigned long double SystemInfo::division(unsigned long double num, int recursionCount, double beDivided)
{
	unsigned long double localNum = num;
	for (int i = 0; i < recursionCount; ++i)
	{
		localNum = localNum / beDivided;
	}
	return localNum;
}
#elif LINUX_OS
ulong SystemInfo::division(ulong num, int recursionCount, double beDivided)
{
	ulong localNum = num;
	for (int i = 0; i < recursionCount; ++i)
	{
		localNum = localNum / beDivided;
	}
	return localNum;
}
#endif
