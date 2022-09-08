#include "logicserver.h"

#include <configmodule/configmanager.h>
#include <configmodule/proxyserverconfig/proxyserverconfig.h>

LogicServer::LogicServer()
{
	initData();
}

LogicServer::LogicServer(int port)
{
	initData();
}

LogicServer::~LogicServer()
{
}

void LogicServer::start()
{
}

void LogicServer::initData()
{
	initInnerClient();
}

void LogicServer::initInnerClient()
{
	if (!CONFIG_MGR->GetProxyServerConfig())
	{
		return;
	}
	const ProxyServerConfigInfo info = *(CONFIG_MGR->GetProxyServerConfig()->getConfigInfo());

	return;
}
