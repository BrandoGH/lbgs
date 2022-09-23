#ifndef __BASE_REDIS_H__
#define __BASE_REDIS_H__

extern "C"
{
#include <hiredis/hiredis.h>
}

#include <servercommon/basedef.h>
#include <boost/function.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>

namespace
{
const int g_nGetValueMaxSize = 128;
}

/*
*	Redis base operator
*	initialization must be call setxxxCallback,currently only the callback method is supported,otherwise an error will be reported
*/

struct CacheServerConnectBaseCfgInfo;
class BaseRedis : public boost::enable_shared_from_this<BaseRedis>
{
	// callback
	typedef boost::function< void(bool /*ok*/, int /*my seq*/) > CallbackStart;
	typedef boost::function< void(
		int	,				/*opType*/
		const char*,		/*key name*/
		const char*,		/*value of key*/
		uint,				/*key size*/
		uint,				/*value size*/
		uint,				/*key expire time(sec)*/
		bool,				/*ok*/
		const char* 		/*redis return string or value, If NULL, usually fail*/
		) > CallbackOp;

public:
	enum EnOpType
	{
		OP_SET_START,
		OP_SET,
		OP_SETNX,
		OP_SETXX,
		OP_SETEX,
		OP_EXPIRE,
		OP_SET_END,

		OP_GET,
		OP_DEL,
		OP_TTL,
		OP_INCR,
		OP_DECR,
	};

	enum EnKeyExpire
	{
		// Default parameter, no effect
		EN_EXPIRE_NULL = -1,
		// Has expired (used to judge)
		EN_EXPIRED = -2,
	};

	struct RedisReturnST
	{
		RedisReturnST()
		{
			memset(m_getData, 0, g_nGetValueMaxSize);
			m_len = 0;
			m_nInteger = 0LL;
		}
		// get [key]
		char m_getData[g_nGetValueMaxSize];
		uint m_len;

		/*
		*	1.del/unlink [key] 
		*	2.ttl [key]
		*/
		long long m_nInteger;
	};

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
	void setOpCallback(CallbackOp callback);

	void setCurServiceSeq(int seq);
	int getCurServiceSeq();

	// redis cmd
	void set(const std::string& key, const char* val, uint keySize, uint valSize);
	void setnx(const std::string& key, const char* val, uint keySize, uint valSize);
	void setxx(const std::string& key, const char* val, uint keySize, uint valSize);
	void setex(const std::string& key, const char* val, uint keySize, uint valSize, int expireSec);
	void expireKey(const std::string& key, int expireSec);
	BaseRedis::RedisReturnST get(const std::string& key);
	BaseRedis::RedisReturnST delKey(const std::string& key, bool delByAync = false);
	BaseRedis::RedisReturnST ttl(const std::string& key);
	BaseRedis::RedisReturnST incr(const std::string& key);
	BaseRedis::RedisReturnST decr(const std::string& key);

HANDLER:
	void onThreadStart(
		const std::string& ip,
		ushort port,
		const CacheServerConnectBaseCfgInfo* timeoutInfo,
		const std::string password);

private:
	bool connect(const std::string& ip, ushort port, const CacheServerConnectBaseCfgInfo* timeoutInfo);
	bool auth(const std::string password);
	void CloseAllHandle();

private:
	redisContext* m_redisCont;
	redisReply* m_redisRep;

	CallbackStart m_callbackStart;
	CallbackOp m_callbackOp;

	int m_nSeq;
	// connect info
	std::string m_strConnectIp;
	ushort m_nConnectPort;

};

#endif
