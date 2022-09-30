#include "dbserverconfig.h"

DBServerConfig::DBServerConfig()
{
}

DBServerConfig::~DBServerConfig()
{
}

int DBServerConfig::init(const std::string& path)
{
	LOAD_NODE("config.dbserver", initBaseCfg);
	return INIT_OK;
}

const DBBaseCfg* DBServerConfig::getDBBaseCfg()
{
	return &m_baseCfg;
}

int DBServerConfig::initBaseCfg(CommonBoost::PTree& rootNode)
{
	CommonBoost::PTree::iterator it = rootNode.begin();
	for (; it != rootNode.end(); ++it)
	{
		CommonBoost::PTree dataNode = it->second;
		m_baseCfg.m_nPort = CAST_TO(ushort, dataNode.get_child("port").data());
		m_baseCfg.m_strIp = dataNode.get_child("ip").data();
		m_baseCfg.m_strUserName = dataNode.get_child("username").data();
		m_baseCfg.m_strDBName = dataNode.get_child("dbname").data();
		m_baseCfg.m_strPassword = dataNode.get_child("password").data();
	}
	return INIT_OK;
}
