#ifndef __GATESERVER_H__
#define __GATESERVER_H__

#include "user.h"

#include <boostmodule/basedef.h>
#include <boost/atomic.hpp>
#include <vector>

struct ConfigInfo;
class GateServer
{
public:
	enum EnUserStatus
	{
		LOGOUT = 2,
	};

public:
	GateServer();
	GateServer(int port);
	~GateServer();

	void start();

SLOTS:
	void onUserError(
		boost::shared_ptr<User> user,
		const CommonBoost::ErrorCode& ec);

HANDLER:
	void onAcceptHandler(
		const CommonBoost::ErrorCode& err,
		const boost::shared_ptr<User>& user
		);
	void onThreadRunAcceptorIOServer();

private:
	void accept();
	void initData();
	void sendServerInfo(const boost::shared_ptr<User>& user);	// 每个客户端连接后发送一个信息，告诉客户端服务端信息，目前发送字节序存储方式，由客户端组装报文

private:
	CommonBoost::IOServer m_server;
	CommonBoost::Acceptor* m_pAcceptor;
	boost::atomic<int> m_nConnectCount;
	int m_nPort;
};

#endif
