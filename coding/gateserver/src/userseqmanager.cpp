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
	// if list have id avalible
	if (!m_lsAsideSeq.empty())
	{
		SetIntCit cit = m_lsAsideSeq.cbegin();
		retSeq = *cit;
		m_lsAsideSeq.erase(cit);
	}
	else
	{
		retSeq = m_nSeq.load();
		m_nSeq += 1;
	}

	return retSeq;
}

void UserSeqManager::pushAsideSeq(int asideSeq)
{
	m_lsAsideSeq.insert(asideSeq);
}

CommonBoost::Mutex& UserSeqManager::getMutex()
{
	return m_mtxList;
}
