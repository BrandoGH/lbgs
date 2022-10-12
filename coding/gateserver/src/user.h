#ifndef __USER_H__
#define __USER_H__

#include <servercommon/boostmodule/basedef.h>
#include <servercommon/boostmodule/signalcommunication.h>
#include <servercommon/basedef.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <boost/atomic.hpp>

#include <logmodule/logdef.h>

class GateServer;
class User : public boost::enable_shared_from_this<User>
{
public:
	enum EnSignalConnectStatus
	{
		CONNECT_ERROR,
		CONNECT_OK,
	};

public:
	User(CommonBoost::IOServer& ioserver, CommonBoost::IOServer& timerServer);
	~User();

	void ayncRead();
	void ayncSend(const byte* data, uint size);
	CommonBoost::SocketPtr& getSocket();
	void getLinkIP(std::string& outIp);
	void getLinkPort(ushort& outPort);
	void closeSocket();

	int slotConnect(GateServer* gateServer);
	void setSeq(int seq);
	int getSeq();
	void checkUserValid();

SIGNALS:
	DEFINE_SIGNAL(void(
		boost::shared_ptr<User>,
		const CommonBoost::ErrorCode&), sigError);
	DEFINE_SIGNAL(void(
		const byte* data,
		uint dataSize, 
		int userSeq), sigSendDataToProxy);

HANDLER:
	void onAyncRead(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);
	void onAyncSend(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);

	void onCheckUserValid();

private:
	// send info to proxy server
	void forwardToProxy(const byte* readOnceMsg, uint msgSize);
	void sendLogoutProtocal(const CommonBoost::ErrorCode& ec);
	bool sendUserError(const CommonBoost::ErrorCode& ec);

private:
	CommonBoost::SocketPtr m_pSocket;
	CommonBoost::StrandPtr m_pStrand;
	CommonBoost::IOServer& m_ioserver;
	byte m_bytesReadBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesOnceMsg[MsgBuffer::g_nOnceMsgSize];
	MsgHeader m_msgHeader;
	MsgEnder m_msgEnder;
	ushort m_nHasReadDataSize;
	// this user seq num
	int m_nSeq;

	// check user
	boost::shared_ptr<CommonBoost::DeadlineTimer> m_pUesrCheckTimer;
	CommonBoost::IOServer& m_timerServer;
	boost::atomic_bool m_bUserValid;
	boost::atomic_bool m_bHasSendError;
};

#endif // !__USER_H__
