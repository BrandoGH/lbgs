#include "sysinfo.h"
#include "logmodule/logdef.h"

#ifdef WIN_OS
#include <windows.h>
#endif 

#ifdef LINUX_OS
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#endif 



SystemInfo::SystemInfo()
{
}

SystemInfo::~SystemInfo()
{
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
unsigned long SystemInfo::getAvailableMemory(int mode)
{
	struct sysinfo info;
	int error = sysinfo(&info);
	if (error != 0)
	{
		LOG_SERVER_COMMON_CONFIG.printLog("error[%d], default return 0", error);
		return 0;
	}
	unsigned long availPhys = info.freeram;

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
unsigned long SystemInfo::division(unsigned long num, int recursionCount, double beDivided)
{
	unsigned long localNum = num;
	for (int i = 0; i < recursionCount; ++i)
	{
		localNum = localNum / beDivided;
	}
	return localNum;
}
#endif
