#include "userseqmanager.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

UserSeqManager::UserSeqManager()
	: m_nSeq(0)
{
}

UserSeqManager::~UserSeqManager()
{
}

ullong UserSeqManager::genSeq(const std::string& ip, ushort port)
{
	ullong retSeq = 0;
	byte* memSeq = (byte*)&retSeq;

	// ip
	std::vector<std::string> vecSplit;
	boost::split(vecSplit, ip, boost::is_any_of("."), boost::token_compress_on);
	for (int i = 0; i < vecSplit.size(); ++i)
	{
		int ipOnceByte = CAST_TO(int, vecSplit[i]);
		memmove(memSeq + i, (byte*)&ipOnceByte, 1);
	}

	// port
	memmove(memSeq + 4, (byte*)&port, sizeof(port));
	
	return retSeq;
}


CommonBoost::Mutex& UserSeqManager::getMutex()
{
	return m_mtx;
}
