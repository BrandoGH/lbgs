#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>
#include <configmodule/proxyserverconfig/proxyserverconfig.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

int main(int argc,char* argv[])
{
	assert(LbgsMinisDump::autoDump() != NULL);
	SystemInfoNS::g_strCurProcessName = boost::filesystem::path(argv[0]).filename().string();

	getchar();
	return 0;
}