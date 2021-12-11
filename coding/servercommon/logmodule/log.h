#ifndef __LOG_H__
#define __LOG_H__

#include "boostcommondef/basedef.h"
#include "basedef.h"

/************************************************************************/
/* 日志模块
/* 注意：每次调用前，先调用setLogHeader()函数，保证记录到局部信息：文件名，函数名，函数等

定义日志步骤:
	1.现在log.cpp namespace CommonLog里定义LogModule变量
	2.在logdef.h 里可以定义宏来简化

/************************************************************************/

namespace
{
const int g_nPrintMaxSize = 1024;
const int g_nPrintHeaderMaxSize = 128;
}

class LogModule
{
public:
	enum Level
	{
		LV_INFO,
		LV_WARNING,
		LV_ERROR,
	};

	enum LogType
	{
		TYPE_DAILY,
		TYPE_ROTATING,
	};

public:
	LogModule(const std::string& logFilePath, int level = LogModule::LV_INFO, int logType = LogModule::TYPE_DAILY);
	~LogModule();

	void setLogDir(const std::string dir);
	LogModule& setLevel(int level);
	LogModule& setLogHeader(
		const char* function,
		const char* file,
		int line,
		int threadId = THREAD_ID
	);
	void printLog(const char* format, ...);

private:
	int m_level;
	int m_logType;
	int m_lastLevel;
	bool m_isTemporaryEffect;

	// daily 更新时间
	int m_nHours;
	int m_nMin;

	// rotating 滚动配置
	int m_rotatingLogMaxSize;
	int g_rotatingLogMaxFiles;

	// header 信息
	std::string m_fileName;
	int m_line;
	std::string m_functionName;
	int m_threadId;

	std::string m_logDir;
	std::string m_logFilePath;

	char m_strPrintHeader[g_nPrintHeaderMaxSize];
	char m_strPrintCont[g_nPrintMaxSize];

	Common::LoggerPtr m_pLog;
	std::string m_logString;

	CommonBoost::Mutex m_contentMutex;
	CommonBoost::Mutex m_headerMutex;
};

namespace CommonLog
{
extern 	std::vector<LogModule*> g_vecLogModule;

extern LogModule g_logGateServer;
extern LogModule g_logServerCommonConfig;
}

#endif // !__LOG_H__
