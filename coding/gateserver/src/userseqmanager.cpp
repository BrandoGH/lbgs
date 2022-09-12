#include "userseqmanager.h"

UserSeqManager::UserSeqManager()
	: m_nSeq(0)
{
}

UserSeqManager::~UserSeqManager()
{
}

int UserSeqManager::getAvailableSeq()
{
	int retSeq = 0;
	// 如果list里有id可用
	if (!m_lsAsideSeq.empty())
	{
		retSeq = m_lsAsideSeq.front();
		m_lsAsideSeq.pop_front();
	}
	else
	{
		retSeq = m_nSeq++;
	}

	return retSeq;
}

void UserSeqManager::pushAsideSeq(int asideSeq)
{
	m_lsAsideSeq.push_front(asideSeq);
}

CommonBoost::Mutex& UserSeqManager::getMutex()
{
	return m_mtxList;
}
