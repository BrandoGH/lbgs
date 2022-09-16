#include "src/gateserver.h"
#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/configmodule/configmanager.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <assert.h>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

void optPort(const boost::program_options::variables_map& vm, GateServer*& pGateServer)
{
	if(vm.count("port"))
	{
		pGateServer = new GateServer(vm["port"].as<uint>());
		printf("new GateServer(int port)\n");
		return;
	}
	pGateServer = new GateServer();
	printf("new GateServer()\n");
}

/*
	command: gateserver --port=xxx 
*/
int main(int argc, char* argv[])
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();

	GateServer* pGateServer = NULL;
	boost::program_options::options_description opts("gateservser option");
	boost::program_options::variables_map vm;
	opts.add_options()
		("help", "get option help")
		("port", boost::program_options::value<uint>(), "set listen port")
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
			pGateServer = new GateServer();
			printf("new GateServer()\n");
			break;
		}

		if(vm.count("help"))
		{
			std::cout << opts << std::endl;
			return 0;
		}

		optPort(vm, pGateServer);

	} while(0);

	assert(pGateServer);
	// start server
	pGateServer->start();
	if(pGateServer)
	{
		printf("GateServer Exit Exception!\n");
		delete pGateServer;
		pGateServer = NULL;
	}


	return 0;
}