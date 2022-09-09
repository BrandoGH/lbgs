#ifndef __GATESERVER_H__
#define __GATESERVER_H__

#include "user.h"
#include "gateserver_aid.h"

#include <boostmodule/basedef.h>
#include <boost/atomic.hpp>
#include "queuemodule/safequeue.hpp"

/*
* ���ط�����
* 
*/

struct GateServerConfigInfo;
class GateServer
{
	friend class User;
	friend class TimerGateProxySrvHeart;
public:
	enum EnUserStatus
	{
		LOGOUT = 2,
	};

public:
	GateServer();
	GateServer(int port);
	~GateServer();

	void start();
	void sendToProxySrv(const byte* data, uint size);

SLOTS:
	void onUserError(
		boost::shared_ptr<User> user,
		const CommonBoost::ErrorCode& ec);
	void OnSendToProxySrvByUser(const byte* data, uint size, boost::shared_ptr<User> sendOriginUser);

HANDLER:
	void onAcceptHandler(
		const CommonBoost::ErrorCode& err,
		const boost::shared_ptr<User>& user
		);
	void onThreadRunAcceptorIOServer();

	// inner client 
	void onConnectInnerServer(const CommonBoost::ErrorCode& err);
	void onProxySrvSend(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);
	void onProxySrvRead(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);

private:
	void accept();
	void initData();

	// ת����
	void initInnerClient();
	void connectInnerServer();
	void closeInnerSocket();
	void runInnnerIOServerOnce();
	void sendServerInfo(const boost::shared_ptr<User>& user);	// ÿ���ͻ������Ӻ���һ����Ϣ�����߿ͻ��˷������Ϣ��Ŀǰ�����ֽ���洢��ʽ���ɿͻ�����װ����
	bool isConnectProxySrvSucc() { return m_bConnectProxySrv; }
	void readFromProxySrv();

private:
	CommonBoost::IOServer m_server;
	CommonBoost::Acceptor* m_pAcceptor;
	boost::atomic<int> m_nConnectCount;
	int m_nPort;

	// �ڲ��ͻ��˲��֣����������ڲ�������
	bool m_bInnerRunOnce;
	bool m_bConnectProxySrv;
	CommonBoost::IOServer m_innerServer;
	CommonBoost::SocketPtr m_pInnerSocket;
	CommonBoost::Endpoint m_innerEndpoint;
	byte m_bytesInnerSrvBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesInnerSrvOnceMsg[MsgBuffer::g_nOnceMsgSize];
	TimerGateProxySrvHeart m_innerSrvHeart;
	ushort m_nHasReadProxyDataSize;
	SafeQueue< boost::shared_ptr<User> > m_queueSendProxySrvUser;
};

#endif
