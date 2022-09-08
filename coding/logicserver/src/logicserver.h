#ifndef __LOGIC_SERVER_H
#define __LOGIC_SERVER_H

#include "logicserver_aid.h"

#include <boostmodule/basedef.h>

class LogicServer
{
public:
	LogicServer();
	~LogicServer();

	void start();
	void sendToProxySrv(const byte* data, uint size);

HANDLER:
	void onConnectInnerServer(const CommonBoost::ErrorCode& err);
	void onRunInnnerIOServerOnce();
	void onProxySrvSend(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);

private:
	// ���ӵ����������
	void initInnerClient();
	void connectInnerServer();
	void closeInnerSocket();

private:
	CommonBoost::IOServer m_innerServer;
	CommonBoost::SocketPtr m_pInnerSocket;
	CommonBoost::Endpoint m_innerEndpoint;
	bool m_bConnectProxySrv;
	bool m_bInnerRunOnce;
	// �ʹ����������
	TimerProxySrvHeart m_innerSrvHeart;
};

#endif  //__LOGIC_SERVER_H
