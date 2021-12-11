#include "config.h"

#include <logmodule/logdef.h>

GateServerConfig::GateServerConfig()
{
}

GateServerConfig::~GateServerConfig()
{
}

int GateServerConfig::init(const std::string & path)
{
	LOAD_NODE("config.gateserver", initInfoCfg);
	return INIT_OK;
}

int GateServerConfig::initInfoCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();
	for (; it != rootNode.end(); ++it)
	{
		CommonBoost::PTree dataNode = it->second;
		
		m_info.port = CAST_TO(ushort, dataNode.get_child("port").data());

		if (m_info.port <= 0 || m_info.port > 65535)
		{
			LOG_SERVER_COMMON_CONFIG.printLog("port read error");
			return ERROR_PORT;
		}
	}

	return INIT_OK;
}
