#ifndef __LOGIC_SERVER_AID_H__
#define __LOGIC_SERVER_AID_H__

#include <servercommon/timermodule/timer.h>

class LogicServer;
class [[deprecated]] TimerLogicProxySrvHeart : public Timer
{
public:
	TimerLogicProxySrvHeart();
	~TimerLogicProxySrvHeart();

	void setLogicServer(LogicServer* pLogicServer);
	virtual void timeoutRun();

private:
	LogicServer* m_pLogicServer;
};

#endif // !__LOGIC_SERVER_AID_H__
