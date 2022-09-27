#include "globallogicserver.h"
#include "logicserver.h"

#include <logmodule/logdef.h>
#include <msgmodule/msgcommondef.h>
#include <boostmodule/basedef.h>

GlobalLogicServer::GlobalLogicServer()
{
}

GlobalLogicServer::~GlobalLogicServer()
{
	deleteInstance();
}

GlobalLogicServer* GlobalLogicServer::instanceObj = new(std::nothrow) GlobalLogicServer;
GlobalLogicServer* GlobalLogicServer::instance()
{
	return instanceObj;
}

void GlobalLogicServer::registerGlobal(LogicServer* pLogicServer)
{
	m_globLogicServer = pLogicServer;
}

LogicServer* GlobalLogicServer::getLogicServer()
{
	return m_globLogicServer;
}

void GlobalLogicServer::deleteInstance()
{
	if (instanceObj)
	{
		delete instanceObj;
		instanceObj = NULL;
	}
}
