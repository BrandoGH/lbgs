#ifndef __USER_SEQ_MANAGER_H__
#define __USER_SEQ_MANAGER_H__

#include <list>
#include <boost/atomic/atomic.hpp>
#include <servercommon/boostmodule/basedef.h>

/*
*	
*	UserSeqManager
*	Not thread safe (provides mutex for external calls)
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
	// After assigning the seq to the client, if the client goes offline, the seq will be placed in this list and used by the next connected client
	std::list< int > m_lsAsideSeq;
	CommonBoost::Mutex m_mtxList;
};

#endif
