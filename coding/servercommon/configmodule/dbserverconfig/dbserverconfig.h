#ifndef __DBSERVER_CONFIG_H__
#define __DBSERVER_CONFIG_H__
#include "basedef.h"
#include "configmodule/configinterface.h"

struct DBBaseCfg
{
	DBBaseCfg()
	{
		m_nPort = 0;
		m_strIp = "";
		m_strUserName = "";
		m_strDBName = "";
		m_strPassword = "";
	}
	ushort m_nPort;
	std::string m_strIp;
	std::string m_strUserName;
	std::string m_strDBName;
	std::string m_strPassword;

};

class DBServerConfig : public ConfigInterface
{
public:
	DBServerConfig();
	virtual ~DBServerConfig();

	virtual int init(const std::string& path) override;

	const DBBaseCfg* getDBBaseCfg();

private:
	int initBaseCfg(CommonBoost::PTree& rootNode);

private:
	DBBaseCfg m_baseCfg;
};

#endif // !__DBSERVER_CONFIG_H__
