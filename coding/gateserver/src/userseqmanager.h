#ifndef __USER_SEQ_MANAGER_H__
#define __USER_SEQ_MANAGER_H__

#include <list>
#include <boost/atomic/atomic.hpp>
#include <servercommon/boostmodule/basedef.h>

/*
*	�û���������ŷ�����
*	���̰߳�ȫ(�ṩmutex���ⲿ����)
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
	// ���ͻ��˷���seq������ͻ��������ˣ�seq�ͻ�ŵ����list������һ�����ӵĿͻ���ʹ��
	std::list< int > m_lsAsideSeq;
	CommonBoost::Mutex m_mtxList;
};

#endif
