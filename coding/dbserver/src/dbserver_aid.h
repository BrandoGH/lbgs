#ifndef __DB_SERVER_AID_H__
#define __DB_SERVER_AID_H__

#include <servercommon/timermodule/timer.h>

class DBServer;
class TimerDBProxySrvHeart : public Timer
{
public:
	TimerDBProxySrvHeart();
	~TimerDBProxySrvHeart();

	void setDBServer(DBServer* pDBServer);
	virtual void timeoutRun();

private:
	DBServer* m_pDBServer;
};

#endif // !__DB_SERVER_AID_H__
