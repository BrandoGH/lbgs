#include "src/proxyserver.h"

#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <configmodule/proxyserverconfig/proxyserverconfig.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>


void optPort(const boost::program_options::variables_map& vm, ProxyServer*& pProxyServer)
{
	if(vm.count("port"))
	{
		pProxyServer = new ProxyServer(vm["port"].as<uint>());
		printf("new ProxyServer(int port)\n");
		return;
	}
	pProxyServer = new ProxyServer();
	printf("new ProxyServer()\n");
}

void optLogDir(const boost::program_options::variables_map& vm)
{
	if (vm.count("logdir"))
	{
		for (int i = 0; i < CommonLog::g_vecLogModule.size(); ++i)
		{
			if (!CommonLog::g_vecLogModule[i])
			{
				printf("CommonLog::g_vecLogModule[%d] is NULL!!\n", i);
				continue;
			}
			CommonLog::g_vecLogModule[i]->setLogDir(vm["logdir"].as<std::string>());
		}
	}
}

/*
	command: proxyserver --port=xxx --logdir=xxx
*/
int main(int argc,char* argv[])
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();

	ProxyServer* pProxySrv = NULL;
	boost::program_options::options_description opts("ProxyServer option");
	boost::program_options::variables_map vm;
	opts.add_options()
		("help", "get option help")
		("port", boost::program_options::value<uint>(), "set listen port")
		("logdir", boost::program_options::value<std::string>(), "set log output path dir")
		;
	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);
	}
	catch(...)
	{
		printf("WTF about your input param, please type --help\n");
		return 0;
	}

	do
	{
		if(vm.empty())
		{
			pProxySrv = new ProxyServer();
			printf("new ProxyServer()\n");
			break;
		}

		if(vm.count("help"))
		{
			std::cout << opts << std::endl;
			return 0;
		}

		optLogDir(vm);
		optPort(vm, pProxySrv);
	} while(0);


	assert(pProxySrv);
	pProxySrv->start();
	if(pProxySrv)
	{
		printf("ProxyServer Exit Exception!\n");
		delete pProxySrv;
		pProxySrv = NULL;
	}


	return 0;
}