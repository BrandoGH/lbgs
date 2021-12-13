#ifndef __USER_H__
#define __USER_H__

#include <servercommon/boostcommondef/basedef.h>
#include <servercommon/basedef.h>
#include <boost/atomic.hpp>

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
	void ayncSend(const char* str, uint size);
	CommonBoost::SocketPtr& getSocket();

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
	char m_readBuffer[UserBuffer::g_nReadBufferSize];
 	boost::atomic<int> m_msgCount;
};

#endif // !__USER_H__
