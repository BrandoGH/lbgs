#ifndef __PROXY_SERVER_H__
#define __PROXY_SERVER_H__

#include <boostmodule/basedef.h>
#include <boost/atomic.hpp>

class ProxyServer
{
public:
	ProxyServer();
	ProxyServer(int port);
	~ProxyServer();

private:
	void initData();

private:
	CommonBoost::IOServer m_server;
	CommonBoost::Acceptor* m_pAcceptor;
	int m_nPort;

};

#endif // !__PROXY_SERVER_H__
