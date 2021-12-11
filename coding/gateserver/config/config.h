#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <configmodule/configinterface.h>

struct ConfigInfo
{
	ushort port;
};

class GateServerConfig : public ConfigInterface
{
public:
	enum ErrorCode
	{
		ERROR_PORT = 1,
	};

public:
	GateServerConfig();
	virtual ~GateServerConfig();

	virtual int init(const std::string& path);

	const ConfigInfo* getConfigInfo() { return &m_info; }

private:
	int initInfoCfg(CommonBoost::PTree& rootNode);

private:
	ConfigInfo m_info;
};

#endif // !__CONFIG_H__
