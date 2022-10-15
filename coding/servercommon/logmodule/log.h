#ifndef __LOG_H__
#define __LOG_H__

/************************************************************************/
/* log module
	Note: Before each call, call the setLogHeader() function first to ensure that local information is recorded: file name, function name, function, etc.

	Define log steps:
	1. Now define the LogModule variable in the [log.cpp namespace CommonLog]
	2. Macros can be defined in [logdef.h] to simplify

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

	// daily update time
	int m_nHours;
	int m_nMin;

	// rotating [rolling configuration]
	int m_nRotatingLogMaxSize;
	int m_nRotatingLogMaxFiles;

	// log header info
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

	CommonBoost::Mutex m_mtx;
};

namespace CommonLog
{
extern 	std::vector<LogModule*> g_vecLogModule;

extern LogModule g_logGateServer;
extern LogModule g_logServerCommonConfig;
extern LogModule g_logProxyServer;
extern LogModule g_logLogicServer;
extern LogModule g_logCacheServer;
extern LogModule g_logDBServer;
extern LogModule g_logServerCommon;

extern LogModule g_logRole;
}

#endif // !__LOG_H__
