#ifndef __SYSINFO_H__
#define __SYSINFO_H__

class SystemInfo
{
public:
	enum ShowUnit
	{
		UNIT_B,
		UNIT_K,
		UNIT_M,
		UNIT_G,
	};

public:
	SystemInfo();
	~SystemInfo();

#ifdef WIN_OS
	static unsigned long double getAvailableMemory(int mode = SystemInfo::UNIT_B);
#elif LINUX_OS
	static ulong getAvailableMemory(int mode = SystemInfo::UNIT_B);
#endif

private:
#ifdef WIN_OS
	static unsigned long double division(unsigned long double num, int recursionCount = 1, double beDivided = 1024.0);
#elif LINUX_OS
	static ulong division(ulong num, int recursionCount = 1, double beDivided = 1024.0);
#endif
};

#endif // !__SYSINFO_H__
