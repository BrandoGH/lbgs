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
		const boost::weak_ptr<ServerLinker>& linker,
		const CommonBoost::ErrorCode& ec);
	void onLinkerFirstConnect(const boost::weak_ptr<ServerLinker>& linker, int listIndex);
	void onSendToDstServer(int listIndex, const byte* data, uint dataSize);
HANDLER:
	void onAcceptHandler(
		const CommonBoost::ErrorCode& err,
		const boost::weak_ptr<ServerLinker>& linker
	);

private:
	CommonBoost::IOServer m_server;
	CommonBoost::Acceptor* m_pAcceptor;
	int m_nPort;

	// List of all linkers connected to the proxy server
	boost::shared_ptr<ServerLinker> m_linkerList[MsgHeader::F_MAX];
	CommonBoost::Mutex m_mtxLinkerList;

};

#endif // !__PROXY_SERVER_H__
