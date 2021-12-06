#include "configmanager.h"
#include "configinterface.h"
#include <gateserver/config/config.h>
#include <logmodule/logdef.h>

#define NEW_CONFIG(className)	m_iConfig[DENUM(className)] = new className

ConfigManager::ConfigManager()
{
	NEW_CONFIG(GateServerConfig);

	// config path
	m_cfgPath[DENUM(GateServerConfig)] = "server/server.xml";

	// check
	for (int i = 0; i < TYPE_MAX; ++i)
	{
		if (m_iConfig[i] == NULL)
		{
			LOG_SERVER_COMMON_CONFIG.printLog("m_iConfig[%d] is NULL", i);
			BOOST_ASSERT(0); // ²»Ó¦¸Ã
		}
	}

	initAllConfig();
}

ConfigManager::~ConfigManager()
{
	for (int i = 0; i < TYPE_MAX; ++i)
	{
		if (m_iConfig[i])
		{
			delete m_iConfig[i];
			m_iConfig[i] = NULL;
			LOG_SERVER_COMMON_CONFIG.printLog("m_iConfig[%d] has delete", i);
		}
	}
}

ConfigManager* ConfigManager::instance()
{
	static ConfigManager cm;
	return &cm;
}

void ConfigManager::initAllConfig()
{
	for (int i = 0; i < TYPE_MAX; ++i)
	{
		if (!m_iConfig[i])
		{
			continue;
		}

		if (m_iConfig[i]->init(m_cfgPath[i]) != ConfigInterface::INIT_OK)
		{
			LOG_SERVER_COMMON_CONFIG.printLog("m_iConfig[%d] init error,path[%s]", i, m_cfgPath[i].data());
		}

	}

	LOG_SERVER_COMMON_CONFIG.printLog("all config has been init!!!!");
}