#ifndef __REDIS_CLUSTER_H__
#define __REDIS_CLUSTER_H__

#include "baseredis.h"

#include <boostmodule/basedef.h>
#include <boost/atomic/atomic.hpp>
#include <boost/function.hpp>

/*
*	RedisCluster Manager
*	initialization must be call setxxxCallback,currently only the callback method is supported,otherwise an error will be reported
*/

class CacheServerConfig;
class RedisCluster
{
	// callback
	typedef boost::function< void(bool /*ok*/) > CallbackClusterConnected;

public:
	RedisCluster();
	~RedisCluster();

	void startConnectCluster();

	void setConnectedCallback(CallbackClusterConnected callback);

	// redis cluster cmd
	void set(const std::string& key, const char* val, uint keySize, uint valSize = 0);
	void setnx(const std::string& key, const char* val, uint keySize, uint valSize = 0);
	void setxx(const std::string& key, const char* val, uint keySize, uint valSize = 0);
	BaseRedis::RedisReturnST get(const std::string& key);
	bool existsKey(const std::string& key);
	BaseRedis::RedisReturnST delkey(const std::string& key, bool delByAync = false);

HANDLER:
	void OnStartConnectResult(bool ok, int curRedisSeq);
	void OnOpResult(
		int opType,
		const char* opKey,
		const char* opKeySetVal,
		uint keySize,
		uint valSize,
		bool ok,
		const char* str
		);

private:
	// deal with cluster "get xxx"or "set xxx" problem , example : (error) MOVED 12706 172.17.0.1:7002
	BaseRedis::RedisReturnST clusterDataCheck_MOVED(const std::string& checkKey, const BaseRedis::RedisReturnST& checkRetValue, int opRedisFlag);
	void clusterDataCheck_Set(
		int opType,
		const char* opKey,
		const char* opKeySetVal,
		bool ok,
		const char* str,
		uint keySize,
		uint valSize);

private:
	std::vector< boost::shared_ptr<BaseRedis> > m_vecRedisCluster;
	std::map<std::string, int> m_mapClusterInfoIndex;	// "ip:port" -> m_vecRedisCluster index

	CacheServerConfig* m_CfgCache;
	boost::atomic_int m_nRedisConnectCount;
	CallbackClusterConnected m_cbClusterConnected;

	CommonBoost::Mutex m_mtxRedisOp;
};

#endif // !__REDIS_CLUSTER_H__
