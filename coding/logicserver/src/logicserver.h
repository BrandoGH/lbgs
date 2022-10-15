#ifndef __LOGIC_SERVER_H
#define __LOGIC_SERVER_H

#include "logicserver_aid.h"

#include <boostmodule/basedef.h>
#include <msgmodule/msgcommondef.h>

class LogicServer
{
	friend class TimerLogicProxySrvHeart;
public:
	LogicServer();
	~LogicServer();

	void start();
	void sendToProxySrv(const byte* data, uint size);
	void sendToClient(const byte* data, uint size);
	void sendToCache(const byte* data, uint size);
	void sendToDBServer(const byte* data, uint size);

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
	// link to proxy server
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
	byte m_bytesInnerSrvBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesInnerSrvOnceMsg[MsgBuffer::g_nOnceMsgSize];
	int m_nHasReadProxyDataSize;
	// heart msg with proxy server
	TimerLogicProxySrvHeart m_innerSrvHeart;
};

#endif  //__LOGIC_SERVER_H
