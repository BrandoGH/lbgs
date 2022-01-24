#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include <string>

#define DENUM(className) TYPE_##className
#define GET_CONFIG(className)\
	className* Get##className(){ return (className*)m_pArrConfigs[DENUM(className)];}

class ConfigInterface;
class GateServerConfig;
class ProxyServerConfig;

class ConfigManager
{
public:
	enum ConfigType
	{
		DENUM(GateServerConfig),
		DENUM(ProxyServerConfig),


		TYPE_MAX,
	};

public:
	static ConfigManager* instance();

	GET_CONFIG(GateServerConfig)
	GET_CONFIG(ProxyServerConfig)
	
protected:
	ConfigManager();
	~ConfigManager();

private:
	void initAllConfig();

private:
	ConfigInterface* m_pArrConfigs[TYPE_MAX];
	std::string m_cfgPath[TYPE_MAX];
};
#define CONFIG_MGR ConfigManager::instance()


#endif // !__CONFIG_MANAGER_H__
