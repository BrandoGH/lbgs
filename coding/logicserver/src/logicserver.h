#ifndef __LOGIC_SERVER_H
#define __LOGIC_SERVER_H

#include <boostmodule/basedef.h>
#include <msgmodule/msgcommondef.h>
#include <timermodule/timer2.h>

class LogicServer
{
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
	void sendProxyHeartInfo();

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
	boost::shared_ptr<Timer2> m_innerSrvHeart;
};

#endif  //__LOGIC_SERVER_H
