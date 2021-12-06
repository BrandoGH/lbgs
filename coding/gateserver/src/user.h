#ifndef __USER_H__
#define __USER_H__

#include <servercommon/boostcommondef/basedef.h>

namespace UserBuffer
{
	/*
		4M 够了 由于开发机可用内存只有几个G，所以并发socket只有2000多， 如果buffer size适当调低，可达到万级并发读写无压力【已测试过】
	*/
	const int g_nReadBufferSize = 1024 * 1024 * 4;		
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
