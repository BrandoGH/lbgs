#include "src/dbserver.h"

#include <servercommon/basedef.h>
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

	DBServer* pDBServer = new DBServer();
	printf("new DBServer()\n");

	// start server
	pDBServer->start();
	if (pDBServer)
	{
		printf("DBServer Exit Exception!\n");
		delete pDBServer;
		pDBServer = NULL;
	}

	return 0;
}