#include "user.h"
#include "gateserver.h"

#include <servercommon/commontool/msgtool/msgtool.h>

User::User(CommonBoost::IOServer& ioserver)
	: m_nHasReadDataSize(0)
{
	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	memset(m_bytesOnceMsg, 0, sizeof(m_bytesOnceMsg));
	m_pSocket = boost::make_shared<CommonBoost::Socket>(ioserver);
	assert(m_pSocket != NULL);
}

User::~User()
{
	closeSocket();
}

void User::ayncRead()
{
	if (!m_pSocket)
	{
		LOG_GATESERVER.printLog("m_pSocket == NULL");
		return;
	}

	memset(m_bytesReadBuffer, 0, sizeof(m_bytesReadBuffer));
	m_pSocket->async_read_some(
		MSG_BUFFER(m_bytesReadBuffer, sizeof(m_bytesReadBuffer)),
		BIND(&User::onAyncRead, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2)
		);
}

void User::ayncSend(const byte* data, uint size)
{
	if (!m_pSocket)
	{
		LOG_GATESERVER.printLog("m_pSocket == NULL");
		return;
	}

	m_pSocket->async_write_some(
		MSG_BUFFER(data, size),
		BIND(&User::onAyncSend, shared_from_this(), boost::placeholders::_1, boost::placeholders::_2)
	);
}

void User::onAyncRead(
	const CommonBoost::ErrorCode& ec,
	uint readSize
	)
{
	if (ec)
	{
		sigError(shared_from_this(), ec);
		return;
	}
	if (readSize <= 0 || readSize > MsgBuffer::g_nReadBufferSize)
	{
		LOG_GATESERVER.printLog("size error,readSize[%d],g_nReadBufferSize[%d]", readSize, MsgBuffer::g_nReadBufferSize);
		return;
	}

	m_msgHeader.reset();
	m_msgEnder.reset();
	m_nHasReadDataSize = 0;


	/*
		这里只处理粘包，半包由客户端去控制，比如客户端请求协议后启动定时器，如果一段时间没有回应，则重新请求，
		然后也可以规定请求最大次数，具体由客户端决定
	*/
	while(m_nHasReadDataSize < readSize)
	{
		// 分析协议
		m_msgHeader = *(MsgHeader*)(m_bytesReadBuffer + m_nHasReadDataSize);

		// 一条协议最大长度判断
		if(m_msgHeader.m_nMsgLen > MsgBuffer::g_nOnceMsgSize ||
			m_msgHeader.m_nMsgLen <= 0)		
		{
			LOG_GATESERVER.printLog("msgtype[%d] size[%d] error, read buff[%s]",
				m_msgHeader.m_nMsgType, 
				m_msgHeader.m_nMsgLen, 
				m_bytesReadBuffer);
			break;
		}

		memmove(m_bytesOnceMsg, m_bytesReadBuffer + m_nHasReadDataSize, m_msgHeader.m_nMsgLen);
		ushort userDataSize = m_msgHeader.m_nMsgLen - sizeof(MsgHeader) - sizeof(MsgEnder);
		m_msgEnder = *(MsgEnder*)(m_bytesOnceMsg + sizeof(MsgHeader) + userDataSize);

		// MD5校验报文
		DEFINE_BYTE_ARRAY(md5,16);
		CommonTool::MsgTool::data2Md5(m_bytesOnceMsg, sizeof(MsgHeader) + userDataSize, md5);
		if(!CommonTool::MsgTool::isBytesMd5EQ(md5, m_msgEnder.m_bytesMD5) || 
			(m_msgHeader.m_nMsgType < MSG_TYPE_HEART_CS || 
				m_msgHeader.m_nMsgType >= MSG_CODE_MAX))
		{
			// 丢弃此条协议
			LOG_GATESERVER.printLog("msgtype[%d] md5 not eq or msgtype error", m_msgHeader.m_nMsgType);
			m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
			continue;
		}

		// TODO 协议转发给内部服务器 实现一个mq来发送 可以直接用gateserver slotconnet传进来的，user是gateserver的友元
		// (m_bytesOnceMsg,userDataSize,shared_from_this())
		// 重新组装报文 转发协议头 + m_bytesOnceMsg
		// test
		//ayncSend(m_bytesOnceMsg, m_msgHeader.m_nMsgLen);
		//printf("send inner server: %s, readSize: %d\n", m_bytesOnceMsg, m_msgHeader.m_nMsgLen);
		// 
		// 不需要MsgEnder的验证长度了
		forwardToProxy(m_bytesOnceMsg, sizeof(MsgHeader) + userDataSize);

		m_nHasReadDataSize += m_msgHeader.m_nMsgLen;
	}

	ayncRead();
}


CommonBoost::SocketPtr & User::getSocket()
{
	return m_pSocket;
}

void User::getLinkIP(std::string& outIp)
{
	if (m_pSocket)
	{
		outIp = m_pSocket->remote_endpoint().address().to_string();
	}
}

void User::getLinkPort(ushort& outPort)
{
	if (m_pSocket)
	{
		outPort = m_pSocket->remote_endpoint().port();
	}
}

void User::closeSocket()
{
	if (!m_pSocket)
	{
		LOG_GATESERVER.printLog("socket close error!");
		return;
	}
	m_pSocket->close();
}

int User::slotConnect(GateServer* gateServer)
{
	if(!gateServer)
	{
		LOG_GATESERVER.printLog("gateServer == NULL, connect slot error!");
		return CONNECT_ERROR;
	}

	sigError.connect(BIND(
		&GateServer::onUserError,
		gateServer,
		boost::placeholders::_1,
		boost::placeholders::_2));

	sigSendDataToProxy.connect(BIND(
		&GateServer::onSendDataToProxy,
		gateServer,
		boost::placeholders::_1,
		boost::placeholders::_2));

	return CONNECT_OK;
}

void User::onAyncSend(const CommonBoost::ErrorCode & ec, uint readSize)
{
	if (ec)
	{
		LOG_GATESERVER.printLog("error value[%d],send size[%d], message[%s]",
			ec.value(),
			readSize,
			ec.message().data());
	}
}

void User::forwardToProxy(const byte* readOnceMsg, uint msgSize)
{
	MsgHeader* header = (MsgHeader*)readOnceMsg;
	if (!header)
	{
		return;
	}

	header->m_nMsgLen = msgSize;
	header->m_nSender = MsgHeader::F_GATESERVER;
	header->m_nReceiver = MsgHeader::F_LOGICSERVER;
	header->m_nProxyer = MsgHeader::F_PROXYSERVER;


	sigSendDataToProxy(readOnceMsg, msgSize);
}
