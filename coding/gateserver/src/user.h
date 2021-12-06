#ifndef __USER_H__
#define __USER_H__

#include <servercommon/boostcommondef/basedef.h>

namespace UserBuffer
{
	const int g_nReadBufferSize = 1024 * 50;	// 每次读取最多50K	
}

class User : public boost::enable_shared_from_this<User>
{
public:
	User(CommonBoost::IOServer& ioserver);
	~User();

	void ayncRead();
	void ayncSend(const char* str, unsigned int size);
	CommonBoost::SocketPtr& getSocket();

HANDLER:
	void onAyncRead(
		const CommonBoost::ErrorCode& ec,
		unsigned int readSize
	);
	void onAyncSend(
		const CommonBoost::ErrorCode& ec,
		unsigned int readSize
	);

private:
	CommonBoost::SocketPtr m_pSocket;
	char m_readBuffer[UserBuffer::g_nReadBufferSize];
};

#endif // !__USER_H__
