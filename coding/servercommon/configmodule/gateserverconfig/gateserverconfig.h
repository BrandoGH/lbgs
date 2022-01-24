#ifndef __GATESERVER_CONFIG_H__
#define __GATESERVER_CONFIG_H__

#include <configmodule/configinterface.h>

struct GateServerConfigInfo
{
	ushort port;
};

class GateServerConfig : public ConfigInterface
{
public:
	enum EnErrorCode
	{
		ERROR_PORT = 1,
	};

public:
	GateServerConfig();
	virtual ~GateServerConfig();

	virtual int init(const std::string& path) override;

	const GateServerConfigInfo* getConfigInfo() { return &m_info; }

private:
	int initInfoCfg(CommonBoost::PTree& rootNode);

private:
	GateServerConfigInfo m_info;
};

#endif // !__GATESERVER_CONFIG_H__
