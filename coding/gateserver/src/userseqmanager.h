#ifndef __USER_SEQ_MANAGER_H__
#define __USER_SEQ_MANAGER_H__

#include <list>
#include <boost/atomic/atomic.hpp>
#include <servercommon/boostmodule/basedef.h>

/*
*	用户的连接序号分配器
*	非线程安全(提供mutex供外部调用)
*/
class UserSeqManager
{
public:
	UserSeqManager();
	~UserSeqManager();

	int getAvailableSeq();
	void pushAsideSeq(int asideSeq);
	CommonBoost::Mutex& getMutex();

private:
	boost::atomic<int> m_nSeq;
	// 给客户端分配seq后，如果客户端下线了，seq就会放到这个list，给下一个连接的客户端使用
	std::list< int > m_lsAsideSeq;
	CommonBoost::Mutex m_mtxList;
};

#endif
