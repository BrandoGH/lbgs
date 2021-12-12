#ifndef __GATESERVER_H__
#define __GATESERVER_H__

#include "user.h"

#include <boostcommondef/basedef.h>
#include <boost/atomic.hpp>
#include <vector>

struct ConfigInfo;
class GateServer
{
public:
	GateServer();
	GateServer(int port);
	~GateServer();

	void start();

private:
	void accept();
	void initData();
	void initData2();

HANDLER:
	void onAcceptHandler(
		const CommonBoost::ErrorCode& err,
		const boost::shared_ptr<User>& user
		);
	void onThreadRunAcceptorIOServer();

private:
	CommonBoost::IOServer m_server;
	CommonBoost::Acceptor* m_pAcceptor;
	const ConfigInfo* m_pCfgInfo;
	boost::atomic<int> m_nConnectCount;
	int m_nPort;
};

#endif
