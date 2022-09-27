#ifndef __GLOABAL_LOGIC_SERVER_H__
#define __GLOABAL_LOGIC_SERVER_H__
#include <servercommon/basedef.h>

class LogicServer;
class GlobalLogicServer
{
public:
	GlobalLogicServer();
	~GlobalLogicServer();
	static GlobalLogicServer* instance();

	void registerGlobal(LogicServer* pLogicServer);
	LogicServer* getLogicServer();

private:
	void deleteInstance();

private:
	static GlobalLogicServer* instanceObj;

	LogicServer* m_globLogicServer;
};

#define GLOBAL_LOGIC GlobalLogicServer::instance()

#endif // !__GLOABAL_LOGIC_SERVER_H__
