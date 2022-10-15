#include "ioserverpool.h"
#include "servercommon/basedef.h"
#include "logmodule/logdef.h"

IOServerPool::IOServerPool()
	: m_nServerIndex(0)
{
}

IOServerPool::~IOServerPool()
{
}

void IOServerPool::init()
{
	for (int i = 0; i < CPU_MAX_THREAD; ++i)
	{
		boost::shared_ptr<CommonBoost::IOServer> pIOServer = boost::make_shared<CommonBoost::IOServer>();
		m_vecIOServer.push_back(pIOServer);
	}

	for (int i = 0; i < CPU_MAX_THREAD; ++i)
	{
		m_vecThread.emplace_back(BIND(&IOServerPool::runIOServer, this, i));
		m_vecThread[i].detach();
	}
}

CommonBoost::IOServer& IOServerPool::getIOServer()
{
	m_nServerIndex = m_nServerIndex >= CPU_MAX_THREAD ? 0 : m_nServerIndex;
	CommonBoost::IOServer& ios = *m_vecIOServer[m_nServerIndex++];
	return ios;
}

void IOServerPool::runIOServer(int vecIndex)
{
	if (!m_vecIOServer[vecIndex])
	{
		LOG_SERVER_COMMON.printLog("m_vecIOServer[%d] NULL", vecIndex);
		return;
	}
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(*m_vecIOServer[vecIndex]));

	/*
	If an exception is caught, an error may occur. The reason for this error is that the connection is disconnected immediately after the client establishes the connection,
	before the server calls remote_endpoint, resulting in a return failure.

	Error info£º
		terminate called after throwing an instance of 'boost::wrapexcept<boost::system::system_error>'
		what():  remote_endpoint: Transport endpoint is not connected
		Aborted
	*/

	while (1)
	{
		THREAD_SLEEP(1);
		try
		{
			m_vecIOServer[vecIndex]->run();
			break;
		} catch (std::exception& e)
		{
			LOG_SERVER_COMMON.printLog("m_innerServer run exception!! info[%s] server will re-start!!", e.what());
			printf_color(PRINTF_RED, "%s : m_innerServer run exception!! info[%s] server will re-start!!\n", __FUNCTION__, e.what());
		}
	}
}
