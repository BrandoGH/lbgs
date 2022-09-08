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

int main(int argc, char* argv[])
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();

	LogicServer* pLogicServer = new LogicServer();
	printf("new LogicServer()\n");

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