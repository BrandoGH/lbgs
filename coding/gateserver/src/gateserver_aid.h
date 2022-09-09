#ifndef __GATESERVER_AID_H__
#define __GATESERVER_AID_H__

#include <servercommon/timermodule/timer.h>

class GateServer;
class TimerGateProxySrvHeart : public Timer
{
public:
	TimerGateProxySrvHeart();
	~TimerGateProxySrvHeart();

	void setGateServer(GateServer* gateserver);
	virtual void timeoutRun();

private:
	GateServer* m_pGateServer;
};

#endif // !__GATESERVER_AID_H__
