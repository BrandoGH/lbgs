#ifndef __SERVER_LINKER_H__
#define __SERVER_LINKER_H__

#include <servercommon/basedef.h>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/msgmodule/msgcommondef.h>

class ServerLinker : public boost::enable_shared_from_this<ServerLinker>
{
public:
	ServerLinker(CommonBoost::IOServer& ioserver);
	~ServerLinker();

	CommonBoost::SocketPtr& getSocket();
	void ayncRead();

private:
	void closeSocket();

HANDLER:
	void onAyncRead(
		const CommonBoost::ErrorCode& ec,
		uint readSize
	);

private:
	CommonBoost::SocketPtr m_pSocket;
	byte m_bytesReadBuffer[MsgBuffer::g_nReadBufferSize];
};

#endif // !__SERVER_LINKER_H__
