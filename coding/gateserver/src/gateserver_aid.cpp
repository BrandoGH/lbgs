#include "gateserver_aid.h"
#include "gateserver.h"
#include "gateservermsg/gateservermsg.h"

#include <servercommon/logmodule/logdef.h>

TimerProxySrvHeart::TimerProxySrvHeart()
	: m_pGateServer(NULL)
{
}

TimerProxySrvHeart::~TimerProxySrvHeart()
{
}

void TimerProxySrvHeart::setGateServer(GateServer* gateserver)
{
	m_pGateServer = gateserver;
}

void TimerProxySrvHeart::timeoutRun()
{
	if(!m_pGateServer)
	{
		LOG_GATESERVER.printLog("m_pGateServer NULL");
		return;
	}

	MsgInHeartCS msg;
	memmove(msg.m_bytesHeart, "\x4C\x42\x47\x53", sizeof(msg.m_bytesHeart));

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + sizeof(msg.m_bytesHeart);
	header.m_nMsgType = MSG_TYPE_GATE_PROXY_HEART_GP;
	header.m_nSender = MsgHeader::F_GATESERVER;
	header.m_nReceiver = MsgHeader::F_PROXYSERVER;
	header.m_nProxyer = MsgHeader::F_PROXYSERVER;

	DEFINE_BYTE_ARRAY(sendInfo, sizeof(MsgHeader) + sizeof(msg.m_bytesHeart));
	memmove(sendInfo, (const char*)&header, sizeof(MsgHeader));
	memmove(sendInfo + sizeof(MsgHeader), (const char*)&msg, sizeof(MsgInHeartCS));

	m_pGateServer->onSendDataToProxy((const byte*)sendInfo, sizeof(sendInfo));
}
