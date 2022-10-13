#ifndef __USER_SEQ_MANAGER_H__
#define __USER_SEQ_MANAGER_H__

#include <boost/atomic/atomic.hpp>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>
#include <set>

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

	ullong genSeq(const std::string& ip, ushort port);
	CommonBoost::Mutex& getMutex();

private:
	boost::atomic_ullong m_nSeq;
	CommonBoost::Mutex m_mtx;
};

#endif
