#include "log.h"
#include <cstdarg>
#include <assert.h>
#include <vector>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace CommonLog
{
std::vector<LogModule*> g_vecLogModule;

LogModule g_logGateServer("/gateserver/gateserver.log");
LogModule g_logServerCommonConfig("/servercommonconfig/servercommonconfig.log");
LogModule g_logProxyServer("/proxyserver/proxyserver.log");
LogModule g_logLogicServer("/logicserver/logicserver.log");
LogModule g_logCacheServer("/cacheserver/cacheserver.log");
LogModule g_logDBServer("/dbserver/dbserver.log");
}


LogModule::LogModule(const std::string& logFilePath, int level, int logType):
	m_nLevel(level),
	m_nLogType(logType),
	m_nLastLevel(m_nLevel),
	m_bTemporaryEffect(false),
	m_nHours(0),
	m_nMin(0),
	m_nRotatingLogMaxSize(1024 * 1024 * 5),
	m_nRotatingLogMaxFiles(10),
	m_strLogDir(LOG_ROOT_PATH),
	m_strLogFilePath(logFilePath)
{
	memset(m_bytesPrintHeader, 0, sizeof(m_bytesPrintHeader));
	memset(m_bytesPrintCont, 0, sizeof(m_bytesPrintCont));


#ifdef DLOG
	m_pLog = spdlog::stdout_color_mt(GEN_UUID);
#else
	if (m_nLogType == TYPE_DAILY)
	{
		m_pLog = spdlog::daily_logger_mt(GEN_UUID, m_strLogDir + m_strLogFilePath, m_nHours, m_nMin);
	}
	else if (m_nLogType == TYPE_ROTATING)
	{
		m_pLog = spdlog::rotating_logger_mt(GEN_UUID, m_strLogDir + m_strLogFilePath, m_nRotatingLogMaxSize, m_nRotatingLogMaxFiles);
	}
#endif
	assert(m_pLog);

	m_pLog->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l]%v");

	CommonLog::g_vecLogModule.push_back(this);
}

LogModule::~LogModule()
{

}

void LogModule::setLogDir(const std::string dir)
{
	m_strLogDir = dir;

	Common::LoggerPtr log;
#ifdef DLOG
	log = spdlog::stdout_color_mt(GEN_UUID);
#else
	if (m_nLogType == TYPE_DAILY)
	{
		log = spdlog::daily_logger_mt(GEN_UUID, m_strLogDir + m_strLogFilePath, m_nHours, m_nMin);
	}
	else if (m_nLogType == TYPE_ROTATING)
	{
		log = spdlog::rotating_logger_mt(GEN_UUID, m_strLogDir + m_strLogFilePath, m_nRotatingLogMaxSize, m_nRotatingLogMaxFiles);
	}
#endif
	if (log)
	{
		m_pLog = log;
	}

	assert(m_pLog);

	m_pLog->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l]%v");

}

LogModule& LogModule::setLevel(int level)
{
	if (m_nLevel >= LV_INFO && m_nLevel <= LV_ERROR)
	{
		m_nLevel = level;
	}
	return *this;
}

LogModule& LogModule::setLogHeader(
	const char* function,
	const char* file,
	int line,
	int threadId
)
{
	CommonBoost::UniqueLock lock(m_mtxHeader);
	m_strFunctionName = function;
	m_strFileName = file;
	m_nLine = line;
	m_nThreadId = threadId;
	return *this;
}

void LogModule::printLog(const char * format, ...)
{
	if (m_pLog.get() == NULL)
	{
		printf("[%s:%d-%s thread(%d)]: m_pLog.get() == NULL",
			__FILE__,__LINE__,__FUNCTION__, THREAD_ID);
		return;
	}
	CommonBoost::UniqueLock lock(m_mtxContent);
	if (m_nLevel < LV_INFO || m_nLevel > LV_ERROR)
	{
		m_nLevel = LV_INFO;
	}

	memset(m_bytesPrintCont, 0, sizeof(m_bytesPrintCont));
	memset(m_bytesPrintHeader, 0, sizeof(m_bytesPrintHeader));

	va_list args;
	va_start(args, format);
	vsnprintf(m_bytesPrintCont, g_nPrintMaxSize, format, args);
	va_end(args);
	sprintf(m_bytesPrintHeader, "[%s:%d-%s thread(%d)]:  ", m_strFileName.data(), m_nLine, m_strFunctionName.data(), m_nThreadId);

	if (strlen(m_bytesPrintHeader) >= g_nPrintHeaderMaxSize ||
		strlen(m_bytesPrintCont) >= g_nPrintMaxSize
		)
	{
		printf("[%s:%d-%s thread(%d)]:print error! strlen(g_strPrintHeader)[%d],strlen(g_strPrintCont)[%d]\n", 
			__FILE__,
			__LINE__, 
			__FUNCTION__,
			THREAD_ID,
			int(strlen(m_bytesPrintHeader)),
			int(strlen(m_bytesPrintCont))
			);
		return;
	}

	m_strLogString.clear();
	m_strLogString.append(m_bytesPrintHeader).append(m_bytesPrintCont);

	switch (m_nLevel)
	{
	case LV_INFO:
		m_pLog->info(m_strLogString);
		break;

	case LV_WARNING:
		m_pLog->warn(m_strLogString);
		break;

	case LV_ERROR:
		m_pLog->error(m_strLogString);
		break;
	}

	m_pLog->flush();
}
