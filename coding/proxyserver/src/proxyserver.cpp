#include "proxyserver.h"
#include <configmodule/proxyserverconfig/proxyserverconfig.h>
#include <servercommon/logmodule/logdef.h>


ProxyServer::ProxyServer()
{
	initData();
	//const ProxyServerConfigInfo* pCfgInfo = CONFIG_MGR->GetProxyServerConfig()->getConfigInfo();
	//if(CONFIG_MGR->GetProxyServerConfig() && pCfgInfo)
	//{
	//	m_nPort = pCfgInfo->port;
	//	m_pAcceptor = new CommonBoost::Acceptor(
	//		m_server,
	//		CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
	//	//accept();
	//	LOG_PROXYSERVER.printLog("has run gateserver succ");
	//}
}

ProxyServer::ProxyServer(int port)
{
	initData();
	
}

ProxyServer::~ProxyServer()
{
}

void ProxyServer::initData()
{
	m_pAcceptor = NULL;
	m_nPort = 0;
}
