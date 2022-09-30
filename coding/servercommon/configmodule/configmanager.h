#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include <string>
#include <boost/static_assert.hpp>

#define DENUM(className) TYPE_##className
#define GET_CONFIG(className)\
	className* Get##className(){ return (className*)m_pArrConfigs[DENUM(className)];}

class ConfigInterface;
class GateServerConfig;
class ProxyServerConfig;
class CacheServerConfig;
class DBServerConfig;

class ConfigManager
{
public:
	enum ConfigType
	{
		DENUM(GateServerConfig),
		DENUM(ProxyServerConfig),
		DENUM(CacheServerConfig),
		DENUM(DBServerConfig),


		TYPE_MAX,
	};
	BOOST_STATIC_ASSERT(TYPE_MAX == 4);	// tip current bytes count

public:
	static ConfigManager* instance();

	GET_CONFIG(GateServerConfig)
	GET_CONFIG(ProxyServerConfig)
	GET_CONFIG(CacheServerConfig)
	GET_CONFIG(DBServerConfig)
	
	BOOST_STATIC_ASSERT(TYPE_MAX == 4);

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
