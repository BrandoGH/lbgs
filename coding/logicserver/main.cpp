#include "src/logicserver.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/configmodule/configmanager.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <assert.h>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

void optPort(const boost::program_options::variables_map& vm, LogicServer*& pLogicServer)
{
	if (vm.count("port"))
	{
		pLogicServer = new LogicServer(vm["port"].as<uint>());
		printf("new LogicServer(int port)\n");
		return;
	}
	pLogicServer = new LogicServer();
	printf("new LogicServer()\n");
}

/*
	command: logicserver --port=xxx --xxx=xxx
*/
int main(int argc, char* argv[])
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();

	LogicServer* pLogicServer = NULL;
	boost::program_options::options_description opts("logicservser option");
	boost::program_options::variables_map vm;
	opts.add_options()
		("help", "get option help")
		("port", boost::program_options::value<uint>(), "set listen port")
		;
	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);
	} catch (...)
	{
		printf("WTF about your input param, please type --help\n");
		return 0;
	}

	do
	{
		if (vm.empty())
		{
			pLogicServer = new LogicServer();
			printf("new LogicServer()\n");
			break;
		}

		if (vm.count("help"))
		{
			std::cout << opts << std::endl;
			return 0;
		}

		optPort(vm, pLogicServer);
	} while (0);

	assert(pLogicServer);

	// start server
	pLogicServer->start();
	if (pLogicServer)
	{
		printf("LogicServer Exit Exception!\n");
		delete pLogicServer;
		pLogicServer = NULL;
	}

	return 0;
}