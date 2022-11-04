#include "timer2.h"

#include "servercommon/basedef.h"
#include "logmodule/logdef.h"

#include <iostream>

Timer2::Timer2(CommonBoost::IOServer& ioserver)
	: m_intervalMilliSec(NULL)
	, m_timerServer(ioserver)
	, m_bPause(false)
	, m_bLoop(false)
{
}

Timer2::~Timer2()
{
	stop();
}

void Timer2::start(CallbackTimeout callback, bool inbLoop)
{
	m_callback = callback;

	if (!m_pStrandTimer)
	{
		m_pStrandTimer = boost::make_shared<CommonBoost::Strand>(m_timerServer);
	}
	if (!m_pTimer)
	{
		m_pTimer = boost::make_shared<CommonBoost::DeadlineTimer>(m_timerServer, boost::posix_time::millisec(m_intervalMilliSec));
		m_pTimer->async_wait(
			m_pStrandTimer->wrap(BIND(&Timer2::onTimeout, shared_from_this())));
	}
	m_bLoop = inbLoop;
}

void Timer2::onTimeout()
{
	if (!m_pTimer || 
		!m_pStrandTimer ||
		m_callback.empty())
	{
		LOG_SERVER_COMMON.printLog("********* timeout error ********* ");
		return;
	}

	if (!m_bPause)
	{
		m_callback();
	}

	if (m_bLoop)
	{
		m_pTimer->expires_from_now(boost::posix_time::millisec(m_intervalMilliSec));
		m_pTimer->async_wait(
			m_pStrandTimer->wrap(BIND(&Timer2::onTimeout, shared_from_this())));
	}
}
