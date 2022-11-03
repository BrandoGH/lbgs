#ifndef __TIMER_H__
#define __TIMER_H__

#include "boostmodule/basedef.h"
#include "servercommon/basedef.h"

class [[deprecated]] Timer
{
public:
	Timer();
	~Timer();

	void start();
	void pause();
	void resume();
	void setInterval(uint milliSec) { m_intervalMilliSec = milliSec; }

protected:
	virtual void timeoutRun() = 0;

HANDLER:
	void onTimeout();

private:
	CommonBoost::DeadlineTimer* m_pTimer;
	CommonBoost::IOServer m_timerServer;
	bool m_bStarted;
	bool m_bPause;
	uint m_intervalMilliSec;
};

#endif // !__TIMER_H__
