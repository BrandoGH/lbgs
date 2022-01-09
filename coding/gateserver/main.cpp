#include "src/gateserver.h"
#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/configmodule/configmanager.h>
#include <assert.h>

namespace
{
typedef void(*ParamResolveHandler)(int, int, const std::string&, GateServer*&);
enum EnParam
{
	PARAM_1,
	PARAM_2,
};

// 是否需要忽略传入的参数
bool isIgnoreInputParam(const std::string param)
{
	if(param == "ign")
	{
		return true;
	}
	return false;
}

void param1Handler(
	int paramIdx, 
	int maxArg,
	const std::string& paramStr,
	GateServer*& gateSrv)
{
	if(paramIdx != EnParam::PARAM_1)
	{
		LOG_GATESERVER.printLog("paramIdx != EnParam::PARAM_1");
		return;
	}

	if(maxArg >= 1)
	{
		if(isIgnoreInputParam(paramStr) || paramStr.empty())
		{
			LOG_GATESERVER.printLog("ignore param [port]");
			gateSrv = new GateServer();
			LOG_GATESERVER.printLog("new GateServer()");
			return;
		}
		else
		{
			gateSrv = new GateServer(CAST_TO(int, paramStr));
			LOG_GATESERVER.printLog("new GateServer(port)");			
		}
	}
}

void param2Handler(
	int paramIdx,
	int maxArg,
	const std::string& paramStr,
	GateServer*& gateSrv)
{
	if(paramIdx != EnParam::PARAM_2)
	{
		LOG_GATESERVER.printLog("paramIdx != EnParam::PARAM_2");
		return;
	}

	if(isIgnoreInputParam(paramStr))
	{
		LOG_GATESERVER.printLog("ignore param [logpath]");
		return;
	}

	for(int i = 0; i < CommonLog::g_vecLogModule.size(); ++i)
	{																		
		if(!CommonLog::g_vecLogModule[i])
		{																	
			LOG_GATESERVER.printLog("CommonLog::g_vecLogModule[%d] is NULL!!", i);
			continue;														
		}																	
		CommonLog::g_vecLogModule[i]->setLogDir(paramStr);
	}
}

ParamResolveHandler paramHandlerList[] =
{
	&param1Handler,
	&param2Handler,
};
}

/*
	command: gateserver [port] [logpath]
	如果不需要某个参数，应该设为ign 如 gateserver.exe 4444 ign 
*/
int main(int argc, char* argv[])
{

	GateServer* pGateServer = NULL;

	int paramListLen = CommonTool::getArraySize(paramHandlerList);
	if(argc > 1)
	{
		if(paramListLen != (argc - 1))
		{
			printf("param size != param handler function size\n");
			return -1;
		}

		// 先初始化日志目录[param 2]
		(*paramHandlerList[EnParam::PARAM_2])(EnParam::PARAM_2, argc, argv[EnParam::PARAM_2 + 1], pGateServer);

		for(int i = 0; i < paramListLen; ++i)
		{
			if(i == EnParam::PARAM_2)
			{
				continue;
			}
			(*paramHandlerList[i])(i, argc, argv[i + 1], pGateServer);
		}
	}
	else if(argc == 1)		// 默认参数
	{
		(*paramHandlerList[EnParam::PARAM_1])(EnParam::PARAM_1, argc, "", pGateServer);
	}

	assert(pGateServer);

	CONFIG_MGR;	// 调用单例，初始化配置

	// 启动服务
	pGateServer->start();
	if(pGateServer)
	{
		LOG_GATESERVER.printLog("GateServer Exit Exception!");
		delete pGateServer;
		pGateServer = NULL;
	}

	return 0;
}