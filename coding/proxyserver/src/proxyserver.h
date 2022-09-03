#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__

#include "serverlinker.h"

#include <boostmodule/basedef.h>
#include <boost/atomic.hpp>
#include <servercommon/msgmodule/msgcommondef.h>

class ProxyServer
{
public:
	enum EnUserStatus
	{
		LOGOUT = 2,
	};
public:
	ProxyServer();
	ProxyServer(int port);
	~ProxyServer();

	void start();

private:
	void initData();
	void accept();
	void onThreadRunAcceptorIOServer();

SLOTS:
	void onLinkerError(
		boost::shared_ptr<ServerLinker> linker,
		const CommonBoost::ErrorCode& ec);
	void onLinkerFirstConnect(boost::shared_ptr<ServerLinker> linker, int listIndex);
	void onSendToDstServer(int listIndex, const byte* data, uint dataSize);
HANDLER:
	void onAcceptHandler(
		const CommonBoost::ErrorCode& err,
		const boost::shared_ptr<ServerLinker>& linker
	);

private:
	CommonBoost::IOServer m_server;
	CommonBoost::Acceptor* m_pAcceptor;
	int m_nPort;

	// 和代理服连接的所有linker列表
	boost::shared_ptr<ServerLinker> m_linkerList[MsgHeader::F_MAX];

};

#endif // !__PROXY_SERVER_H__
