#include "sysinfo.h"
#include "logmodule/logdef.h"

#ifdef WIN_OS
#include <windows.h>
#include <tlhelp32.h>
#include <comdef.h>
#elif LINUX_OS
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
	HANDLE info_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Take a snapshot of all processes in the system
	if(info_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32W program_info;
	program_info.dwSize = sizeof(PROCESSENTRY32W);  // set struct size
	int bResult = Process32FirstW(info_handle, &program_info); // get first process info from all process
	if(!bResult)
	{
		return false;
	}

	while(bResult)
	{
		THREAD_SLEEP(1);
		std::string pro_name = _bstr_t(program_info.szExeFile);
		if(processName == pro_name)
		{
			ret = true;
			break;
		}
		// get next process info
		bResult = Process32NextW(info_handle,&program_info);
	}
	CloseHandle(info_handle);// close
	return ret;
#elif LINUX_OS
	FILE* fp = NULL;
	char cmdResBuf[100];
	char cmd[200];
	memset(cmdResBuf, 0, sizeof(cmdResBuf));
	memset(cmd,0,sizeof(cmd));

	pid_t pid = -1;
	
	sprintf(cmd, "pidof %s", processName.data());
	if((fp = popen(cmd, "r")) != NULL)
	{
		if(fgets(cmdResBuf, 255, fp) != NULL)
		{
			pid = atoi(cmdResBuf);
		}
	}
	pclose(fp);

	return (pid != -1);
#endif 
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
