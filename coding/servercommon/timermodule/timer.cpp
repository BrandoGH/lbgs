#include "timer.h"

#include "servercommon/basedef.h"

#include <iostream>

Timer::Timer()
	: m_pTimer(NULL)
	, m_bStarted(false)
	, m_bPause(false)
	, m_intervalMilliSec(0)
{
}

Timer::~Timer()
{
	DEL_OBJ(m_pTimer);
}

void Timer::start()
{
	if(m_bStarted)
	{
		return;
	}

	if(!m_pTimer)
	{
		m_pTimer = new CommonBoost::DeadlineTimer(m_timerServer, boost::posix_time::millisec(m_intervalMilliSec));
	}

	m_pTimer->async_wait(BIND(&Timer::onTimeout, this));
	m_bPause = false;

	boost::thread t([&]() {m_timerServer.run(); });
	t.detach();
}

void Timer::pause()
{
	m_bPause = true;
}
void Timer::resume()
{
	m_bPause = false;
}

void Timer::onTimeout()
{
	if(!m_bPause)
	{
		timeoutRun();
	}
	m_pTimer->expires_from_now(boost::posix_time::millisec(m_intervalMilliSec));
	m_pTimer->async_wait(BIND(&Timer::onTimeout, this));
}
