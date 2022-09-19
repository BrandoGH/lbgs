#ifndef __BASE_REDIS_H__
#define __BASE_REDIS_H__

extern "C"
{
#include <hiredis/hiredis.h>
}

#include <servercommon/basedef.h>
#include <boost/function.hpp>


struct CacheServerConnectBaseCfgInfo;
class BaseRedis
{
	// callback
	typedef boost::function< void(bool) > CallbackStart;

public:
	BaseRedis();
	~BaseRedis();

	void start(
		const std::string& ip, 
		ushort port, 
		const CacheServerConnectBaseCfgInfo* timeoutInfo = NULL,
		const std::string password = std::string());
	void end();

	const std::string getRedisServerIp();
	ushort getRedisServerPort();

	void setStartCallback(CallbackStart callback);

HANDLER:
	void onThreadStart(
		const std::string& ip,
		ushort port,
		const CacheServerConnectBaseCfgInfo* timeoutInfo,
		const std::string password);

private:
	bool connect(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo);
	bool auth(const std::string password);
	void CloseRedisContext();

private:
	redisContext* m_redisCont;
	redisReply* m_redisRep;

	CallbackStart m_calllbackStart;

	// connect info
	std::string m_strConnectIp;
	ushort m_nConnectPort;

};

#endif
