#ifndef __SERVER_LINKER_H__
#define __SERVER_LINKER_H__

#include <servercommon/basedef.h>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <servercommon/boostmodule/signalcommunication.h>


class ProxyServer;
class ServerLinker : public boost::enable_shared_from_this<ServerLinker>
{
public:
	enum EnSignalConnectStatus
	{
		CONNECT_ERROR,
		CONNECT_OK,
	};
public:
	ServerLinker(CommonBoost::IOServer& ioserver);
	~ServerLinker();

	CommonBoost::SocketPtr& getSocket();
	void ayncRead();
	void ayncSend(const byte* data, uint size);
	void closeSocket();

	int slotConnect(ProxyServer* proxyServer);
private:

SIGNALS:
	DEFINE_SIGNAL(void(
		boost::shared_ptr<ServerLinker>,
		const CommonBoost::ErrorCode&), sigError);

HANDLER:
	void onAyncRead(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);
	void onAyncSend(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);

private:
	CommonBoost::SocketPtr m_pSocket;
	byte m_bytesReadBuffer[MsgBuffer::g_nReadBufferSize];
	byte m_bytesOnceMsg[MsgBuffer::g_nOnceMsgSize];
	MsgHeader m_msgHeader;
	ushort m_nHasReadDataSize;

};

#endif // !__SERVER_LINKER_H__
