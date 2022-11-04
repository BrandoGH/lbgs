#ifndef __TIMER2_H__
#define __TIMER2_H__

#include "boostmodule/basedef.h"
#include "servercommon/basedef.h"

#include <boost/smart_ptr/enable_shared_from_this.hpp>


class Timer2 : public boost::enable_shared_from_this<Timer2>
{
	// callback
	typedef boost::function< void(void) > CallbackTimeout;
public:
	Timer2(CommonBoost::IOServer& ioserver);
	virtual ~Timer2();

	void setInterval(uint milliSec) { m_intervalMilliSec = milliSec; }
	void start(CallbackTimeout callback, bool inbLoop = true);
	void stop() { m_bLoop = false; };
	void pause() { m_bPause = true; }
	void resume() { m_bPause = false; }

private:
	void onTimeout();

private:
	boost::shared_ptr<CommonBoost::DeadlineTimer> m_pTimer;
	CommonBoost::IOServer& m_timerServer;
	CommonBoost::StrandPtr m_pStrandTimer;
	uint m_intervalMilliSec;
	CallbackTimeout m_callback;
	bool m_bPause;
	bool m_bLoop;
};

#endif // !__TIMER2_H__
