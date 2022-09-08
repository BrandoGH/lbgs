#include "proxyserverconfig.h"

#include <servercommon/logmodule/logdef.h>
#include <servercommon/commontool/commontool.h>

ProxyServerConfig::ProxyServerConfig()
{
}

ProxyServerConfig::~ProxyServerConfig()
{
}

int ProxyServerConfig::init(const std::string& path)
{
	LOAD_NODE("config.proxyserver", initInfoCfg);
	return INIT_OK;
}

int ProxyServerConfig::initInfoCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();
	
	for(; it != rootNode.end(); ++it)
	{
		CommonBoost::PTree dataNode = it->second;

		m_info.ip = dataNode.get_child("ip").data();
		m_info.port = CAST_TO(ushort, dataNode.get_child("port").data());
		m_info.heart_time = CAST_TO(ushort, dataNode.get_child("heart_time").data());

		if(!CommonTool::isIpFormat(m_info.ip))
		{
			LOG_PROXYSERVER.printLog("ip read error");
			return ERROR_IP;
		}
		if(m_info.port <= 0 || m_info.port > 65535)
		{
			LOG_PROXYSERVER.printLog("port read error");
			return ERROR_PORT;
		}
		if (m_info.heart_time <= 0 || m_info.port > 65535)
		{
			LOG_PROXYSERVER.printLog("read heart_time error");
			return ERROR_HEART_TIME;
		}
	}

	return INIT_OK;
}
