#include "src/cacheserver.h"

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

	CacheServer* pCacheServer = new CacheServer();
	printf("new CacheServer()\n");

	// start server
	pCacheServer->start();
	if (pCacheServer)
	{
		printf("CacheServer Exit Exception!\n");
		delete pCacheServer;
		pCacheServer = NULL;
	}

	return 0;
}