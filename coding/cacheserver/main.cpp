#include "src/cacheserver.h"
#include "redis/baseredis.h"

#include <servercommon/basedef.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/configmodule/configmanager.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <servercommon/cacheserverconfig.h>

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


	// redis test
	BaseRedis redis;
	if (CONFIG_MGR->GetCacheServerConfig())
	{
		const CacheReplicatConfigInfo* info = CONFIG_MGR->GetCacheServerConfig()->getSingleMasterReidsCfg();
		const CacheServerConnectBaseCfgInfo* info2 = CONFIG_MGR->GetCacheServerConfig()->getBaseCacheCfg();
		if (info && info2)
		{
			if (redis.connect(info->m_strIp, info->m_nPort, info2) &&
				redis.auth(info->m_strPassword))
			{
				printf_color(PRINTF_GREEN, "redis connect succ !!!!\n");
			}
		}
	}



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