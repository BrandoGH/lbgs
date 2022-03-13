#ifndef __LOG_H__
#define __LOG_H__

/************************************************************************/
/* 日志模块
/* 注意：每次调用前，先调用setLogHeader()函数，保证记录到局部信息：文件名，函数名，函数等

定义日志步骤:
	1.现在log.cpp namespace CommonLog里定义LogModule变量
	2.在logdef.h 里可以定义宏来简化

/************************************************************************/

#include "boostmodule/basedef.h"
#include "basedef.h"

namespace
{
const int g_nPrintMaxSize = 1024;
const int g_nPrintHeaderMaxSize = 512;
}

class LogModule
{
public:
	enum EnLevel
	{
		LV_INFO,
		LV_WARNING,
		LV_ERROR,
	};

	enum EnLogType
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
	int m_nLevel;
	int m_nLogType;
	int m_nLastLevel;
	bool m_bTemporaryEffect;

	// daily 更新时间
	int m_nHours;
	int m_nMin;

	// rotating 滚动配置
	int m_nRotatingLogMaxSize;
	int m_nRotatingLogMaxFiles;

	// header 信息
	std::string m_strFileName;
	int m_nLine;
	std::string m_strFunctionName;
	int m_nThreadId;

	std::string m_strLogDir;
	std::string m_strLogFilePath;

	char m_bytesPrintHeader[g_nPrintHeaderMaxSize];
	char m_bytesPrintCont[g_nPrintMaxSize];

	Common::LoggerPtr m_pLog;
	std::string m_strLogString;

	CommonBoost::Mutex m_mtxContent;
	CommonBoost::Mutex m_mtxHeader;
};

namespace CommonLog
{
extern 	std::vector<LogModule*> g_vecLogModule;

extern LogModule g_logGateServer;
extern LogModule g_logServerCommonConfig;
extern LogModule g_logProxyServer;
}

#endif // !__LOG_H__
