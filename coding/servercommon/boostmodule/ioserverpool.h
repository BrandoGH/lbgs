#ifndef __IOSERVER_POOL_H__
#define __IOSERVER_POOL_H__
#include "boostmodule/basedef.h"
#include <vector>

class IOServerPool
{
public:
	IOServerPool();
	~IOServerPool();

	void init();

	CommonBoost::IOServer& getIOServer();

private:
	void runIOServer(int vecIndex);

private:
	std::vector<CommonBoost::Thread> m_vecThread;
	std::vector< boost::shared_ptr<CommonBoost::IOServer> > m_vecIOServer;

	int m_nServerIndex;
};

#endif // !__IOSERVER_POOL_H__
