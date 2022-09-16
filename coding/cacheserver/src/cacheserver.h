#ifndef __CACHE_SERVER_H
#define __CACHE_SERVER_H

#include "cacheserver_aid.h"

#include <boostmodule/basedef.h>
#include <msgmodule/msgcommondef.h>

class CacheServer
{
	friend class TimerCacheProxySrvHeart;
public:
	CacheServer();
	~CacheServer();

	void start();
	void sendToProxySrv(const byte* data, uint size);

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

private:
	// link proxy server
	void initInnerClient();
	void connectInnerServer();
	void closeInnerSocket();
	void readFromProxySrv();

private:
	CommonBoost::IOServer m_innerServer;
	CommonBoost::SocketPtr m_pInnerSocket;
	CommonBoost::StrandPtr m_pInnerStrand;
	CommonBoost::Endpoint m_innerEndpoint;
	bool m_bConnectProxySrv;
	bool m_bInnerRunOnce;
	byte m_bytesInnerSrvBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesInnerSrvOnceMsg[MsgBuffer::g_nOnceMsgSize];
	ushort m_nHasReadProxyDataSize;
	// heart msg with proxy server
	TimerCacheProxySrvHeart m_innerSrvHeart;
};

#endif  //__CACHE_SERVER_H
