#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include <string>

#define DENUM(className) TYPE_##className
#define GET_CONFIG(className)\
	className* Get##className(){ return (className*)m_iConfig[DENUM(className)];}

class ConfigInterface;
class GateServerConfig;

class ConfigManager
{
public:
	enum ConfigType
	{
		DENUM(GateServerConfig),


		TYPE_MAX,
	};

public:
	static ConfigManager* instance();

	GET_CONFIG(GateServerConfig)
	
protected:
	ConfigManager();
	~ConfigManager();

private:
	void initAllConfig();

private:
	ConfigInterface* m_iConfig[TYPE_MAX];
	std::string m_cfgPath[TYPE_MAX];
};
#define CONFIG_MGR ConfigManager::instance()


#endif // !__CONFIG_MANAGER_H__
