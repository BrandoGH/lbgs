#ifndef __DB_SERVER_H
#define __DB_SERVER_H

#include "dbserver_aid.h"

#include <boostmodule/basedef.h>
#include <msgmodule/msgcommondef.h>

class DBServer
{
	friend class TimerDBProxySrvHeart;
public:
	DBServer();
	~DBServer();

	void start();
	void sendToProxySrv(const byte* data, uint size);
	void sendToCacheServer(const byte* data, uint size);

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

	// sql
	void onSqlConnectResult(bool ok, int errorCode);

private:
	// link proxy server
	void initInnerClient();
	void connectInnerServer();
	void closeInnerSocket();
	void readFromProxySrv();

	// sql
	void initPostgresql();
private:
	CommonBoost::IOServer m_innerServer;
	CommonBoost::SocketPtr m_pInnerSocket;
	CommonBoost::StrandPtr m_pInnerStrand;
	CommonBoost::Endpoint m_innerEndpoint;
	bool m_bConnectProxySrv;
	bool m_bInnerRunOnce;
	byte m_bytesInnerSrvBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesInnerSrvOnceMsg[MsgBuffer::g_nOnceMsgSize];
	int m_nHasReadProxyDataSize;
	// heart msg with proxy server
	TimerDBProxySrvHeart m_innerSrvHeart;
};

#endif  //__DB_SERVER_H
