#include "src/gateserver.h"
#include <servercommon/logmodule/logdef.h>
#include <assert.h>

#define RUN_ARG_PORT(pGateServer)									\
do																	\
{																	\
	if (argc == 2)													\
	{																\
		pGateServer = new GateServer(CAST_TO(int, argv[1]));		\
		LOG_GATESERVER.printLog("new GateServer(port)");			\
	}																\
	else                                                            \
	{																\
		pGateServer = new GateServer();								\
		LOG_GATESERVER.printLog("new GateServer()");				\
	}																\
} while (0)

#define RUN_ARG_LOGPATH(vecLogModule)											\
do																				\
{																				\
	if (argc == 3)																\
	{																			\
		std::string logDir = argv[2];											\
		for (int i = 0; i < vecLogModule.size(); ++i)							\
		{																		\
			if (!vecLogModule[i])												\
			{																	\
				LOG_GATESERVER.printLog("%s[%d] is NULL!!", #vecLogModule, i);	\
				continue;														\
			}																	\
			vecLogModule[i]->setLogDir(logDir);									\
		}																		\
	}																			\
} while (0)

int main(int argc, char* argv[])
{
	// ¸ñÊ½: binary port logdir[xxxx/logÂ·¾¶]
	RUN_ARG_LOGPATH(CommonLog::g_vecLogModule);

	GateServer* pGateServer = NULL;
	RUN_ARG_PORT(pGateServer);

	assert(pGateServer);
	pGateServer->start();

	if (pGateServer)
	{
		LOG_GATESERVER.printLog("GateServer Exit Exception!");
		delete pGateServer;
		pGateServer = NULL;
	}

	return 0;
}