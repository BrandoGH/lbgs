#ifndef __BASE_REDIS_H__
#define __BASE_REDIS_H__

extern "C"
{
#include <hiredis/hiredis.h>
}

#include <servercommon/basedef.h>


struct CacheServerConnectBaseCfgInfo;
class BaseRedis
{
public:
	BaseRedis();
	~BaseRedis();

	bool connect(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo);
	bool auth(const std::string password);

private:
	redisContext* m_redisCont;
	redisReply* m_redisRep;

	// connect info
	std::string m_strConnectIp;
	ushort m_nConnectPort;

};

#endif
