#ifndef __LOG_H__
#define __LOG_H__

/************************************************************************/
/* ��־ģ��
/* ע�⣺ÿ�ε���ǰ���ȵ���setLogHeader()��������֤��¼���ֲ���Ϣ���ļ�������������������

������־����:
	1.����log.cpp namespace CommonLog�ﶨ��LogModule����
	2.��logdef.h ����Զ��������

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

	// daily ����ʱ��
	int m_nHours;
	int m_nMin;

	// rotating ��������
	int m_nRotatingLogMaxSize;
	int m_nRotatingLogMaxFiles;

	// header ��Ϣ
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
