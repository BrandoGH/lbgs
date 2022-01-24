#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/configmodule/configmanager.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <configmodule/proxyserverconfig/proxyserverconfig.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

int main(int argc,char* argv[])
{
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();
	ProxyServerConfig* cfg = CONFIG_MGR->GetProxyServerConfig();
	if(!cfg)
	{
		assert(0);
	}
	const ProxyServerConfigInfo* info = cfg->getConfigInfo();
	if(info)
	{
		printf("port: %d\n",info->port);
	}
	getchar();
	return 0;
}