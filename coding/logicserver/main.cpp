#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/configmodule/configmanager.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <assert.h>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

/*
	command: logicserver --port=xxx --xxx=xxx
*/
int main(int argc, char* argv[])
{
#ifdef WIN_OS
	assert(LbgsMinisDump::autoDump() != NULL);
#endif
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();

	while (1)
	{
		LOG_LOGICSERVER.printLog("my process name is [%s]\n", SystemInfoNS::g_strCurProcessName.data());
		THREAD_SLEEP(1000);
	}

	return 0;
}