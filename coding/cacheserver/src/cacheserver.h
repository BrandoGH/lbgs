#ifndef __CACHE_SERVER_H
#define __CACHE_SERVER_H

#include "cacheserver_aid.h"
#include "redis/rediscluster.h"

#include <boostmodule/basedef.h>
#include <msgmodule/msgcommondef.h>
#include <servercommon/cacheserverconfig.h>


class CacheServer
{
	friend class TimerCacheProxySrvHeart;
public:
	CacheServer();
	~CacheServer();

	void start();
	void sendToProxySrv(const byte* data, uint size);
	void sendToDBServer(const byte* data, uint size);
	void sendToLogicServer(const byte* data, uint size);

	RedisCluster* getRedisCluster();

HANDLER:
	void onConnectInnerServer(const CommonBoost::ErrorCode& err);
	void onRunInnnerIOServerOnce();
	void onProxySrvSend(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);
	void onProxySrvRead(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);

	void onRedisClusterConnected(bool ok);

private:
	// link proxy server
	void initInnerClient();
	void connectInnerServer();
	void closeInnerSocket();
	void readFromProxySrv();

	// redis
	void initRedisCluster();

private:
	CommonBoost::IOServer m_innerServer;
	CommonBoost::SocketPtr m_pInnerSocket;
	CommonBoost::StrandPtr m_pInnerStrand;
	CommonBoost::Endpoint m_innerEndpoint;
	bool m_bConnectProxySrv;
	byte m_bytesInnerSrvBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesInnerSrvOnceMsg[MsgBuffer::g_nOnceMsgSize];
	int m_nHasReadProxyDataSize;
	int m_nNextNeedReadSize;
	int m_nLastHasReadSize;
	bool m_bHeaderIntegrated;
	// heart msg with proxy server
	TimerCacheProxySrvHeart m_innerSrvHeart;

	// redis cluster
	RedisCluster m_redisCluster;

};

#endif  //__CACHE_SERVER_H
