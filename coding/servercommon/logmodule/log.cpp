#include "log.h"
#include <cstdarg>
#include <assert.h>
#include <vector>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

namespace 
{
const char* g_strDailyLogName = "dailyLog";
const char* g_strRotatingLogName = "RotatingLog";
}

namespace CommonLog
{
std::vector<LogModule*> g_vecLogModule;

LogModule g_logGateServer("/gateserver/gateserver.log");
LogModule g_logServerCommonConfig("/servercommonconfig/servercommonconfig.log");
}


LogModule::LogModule(const std::string& logFilePath, int level, int logType):
	m_level(level),
	m_logType(logType),
	m_lastLevel(m_level),
	m_isTemporaryEffect(false),
	m_nHours(0),
	m_nMin(0),
	m_rotatingLogMaxSize(1024 * 1024 * 5),
	g_rotatingLogMaxFiles(10),
	m_logDir(LOG_ROOT_PATH),
	m_logFilePath(logFilePath)
{
	memset(m_strPrintHeader, 0, sizeof(m_strPrintHeader));
	memset(m_strPrintCont, 0, sizeof(m_strPrintCont));

	if (m_logType == TYPE_DAILY)
	{
		m_pLog = spdlog::daily_logger_mt(g_strDailyLogName + m_logFilePath, m_logDir + m_logFilePath, m_nHours, m_nMin);
	}
	else if (m_logType == TYPE_ROTATING)
	{
		m_pLog = spdlog::rotating_logger_mt(g_strRotatingLogName + m_logFilePath, m_logDir + m_logFilePath, m_rotatingLogMaxSize, g_rotatingLogMaxFiles);
	}

	assert(m_pLog);

	m_pLog->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l]%v");

	CommonLog::g_vecLogModule.push_back(this);
}

LogModule::~LogModule()
{

}

void LogModule::setLogDir(const std::string dir)
{
	m_logDir = dir;

	Common::LoggerPtr log;
	if (m_logType == TYPE_DAILY)
	{
		log = spdlog::daily_logger_mt(g_strDailyLogName + m_logDir, m_logDir + m_logFilePath, m_nHours, m_nMin);
	}
	else if (m_logType == TYPE_ROTATING)
	{
		log = spdlog::rotating_logger_mt(g_strRotatingLogName + m_logDir, m_logDir + m_logFilePath, m_rotatingLogMaxSize, g_rotatingLogMaxFiles);
	}

	if (log)
	{
		m_pLog = log;
	}

	assert(m_pLog);

	m_pLog->set_pattern("[%Y-%m-%d %H:%M:%S:%e][%l]%v");

}

LogModule& LogModule::setLevel(int level)
{
	if (m_level >= LV_INFO && m_level <= LV_ERROR)
	{
		m_level = level;
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
	CommonBoost::UniqueLock lock(m_headerMutex);
	m_functionName = function;
	m_fileName = file;
	m_line = line;
	m_threadId = threadId;
	return *this;
}

void LogModule::printLog(const char * format, ...)
{
	CommonBoost::UniqueLock lock(m_contentMutex);
	if (m_level < LV_INFO || m_level > LV_ERROR)
	{
		m_level = LV_INFO;
	}

	memset(m_strPrintCont, 0, sizeof(m_strPrintCont));
	memset(m_strPrintHeader, 0, sizeof(m_strPrintHeader));

	va_list args;
	va_start(args, format);
	vsnprintf(m_strPrintCont, g_nPrintMaxSize, format, args);
	va_end(args);
	sprintf(m_strPrintHeader, "[%s:%d-%s thread(%d)]:  ", m_fileName.data(), m_line, m_functionName.data(), m_threadId);

	if (strlen(m_strPrintHeader) >= g_nPrintHeaderMaxSize ||
		strlen(m_strPrintCont) >= g_nPrintMaxSize
		)
	{
		printf("[%s:%d-%s thread(%d)]:print error! strlen(g_strPrintHeader)[%d],strlen(g_strPrintCont)[%d]\n", 
			__FILE__,
			__LINE__, 
			__FUNCTION__,
			THREAD_ID,
			int(strlen(m_strPrintHeader)),
			int(strlen(m_strPrintCont))
			);
		return;
	}

	m_logString.clear();
	m_logString.append(m_strPrintHeader).append(m_strPrintCont);

#ifdef DLOG
	printf("%s\n", m_logString.data());

#else
	switch (m_level)
	{
	case LV_INFO:
		m_pLog->info(m_logString);
		break;

	case LV_WARNING:
		m_pLog->warn(m_logString);
		break;

	case LV_ERROR:
		m_pLog->error(m_logString);
		break;
	}

	m_pLog->flush();
#endif
}
