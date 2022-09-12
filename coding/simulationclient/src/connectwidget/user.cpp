#include "user.h"

#include <QtNetwork/QHostAddress>
#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QString>
#include <QtCore/QMetaEnum>

#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msgheart.h>

User::User(
	const QString& ip,
	uint port,
	QObject* parent
	)
	: m_nUserId(0)
	, m_bServerBigEndian(false)
	, m_pSendData(NULL)
{

	m_pTcpSoc = new QTcpSocket(this);
	m_pTcpSoc->connectToHost(
		QHostAddress(ip),
		port);

	connect(m_pTcpSoc, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(m_pTcpSoc, SIGNAL(error(QAbstractSocket::SocketError)), 
		this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(m_pTcpSoc, SIGNAL(readyRead()), this, SLOT(onReadData()));
}

User::~User()
{
	if(m_pTcpSoc)
	{
		m_pTcpSoc->close();
	}
}

void User::sendData(const QByteArray& msg)
{
	if(!m_pTcpSoc)
	{
		return;
	}

	if(msg == "heart")	// 心跳包
	{
		sendHeartInfo();
	}
	else
	{
		m_pTcpSoc->write(msg);
	}
}

void User::sendData(const char* data, uint dataSize, int msgType)
{
	if(!m_pTcpSoc)
	{
		DEL_ARR(m_pSendData);
		return;
	}

	MsgHeader header;
	header.m_nMsgLen = sizeof(MsgHeader) + dataSize + sizeof(MsgEnder);
	header.m_nMsgType = msgType;
	DEL_ARR(m_pSendData);
	m_pSendData = new byte[header.m_nMsgLen];
	MsgEnder ender;
	auto lambLocalStoreModeSend = [&]()
	{
		memmove(m_pSendData, (const char*)&header, sizeof(MsgHeader));
		memmove(m_pSendData + sizeof(MsgHeader), data, dataSize);
		if(CommonTool::MsgTool::data2Md5(m_pSendData, sizeof(MsgHeader) + dataSize, ender.m_bytesMD5))
		{
			memmove(m_pSendData + sizeof(MsgHeader) + dataSize, (const char*)&ender, sizeof(MsgEnder));
			m_pTcpSoc->write((const char*)m_pSendData, header.m_nMsgLen);
			//m_pTcpSoc->write((const char*)m_pSendData, header.m_nMsgLen);		// 模拟粘包
			//m_pTcpSoc->write("\x18\x00\x00\x00\x47\x42", 6);					// 模拟半包
			emit sigSendData(QByteArray((const char*)m_pSendData, header.m_nMsgLen), getUserId());
		}
	};

	auto lambByteSeqTransformSend = [&]()
	{
		DEFINE_BYTE_ARRAY(headerPackLen, sizeof(header.m_nMsgLen));
		DEFINE_BYTE_ARRAY(headerMsgType, sizeof(header.m_nMsgType));

		CommonTool::MsgTool::byteSeqTransformN2B(header.m_nMsgLen, headerPackLen);
		CommonTool::MsgTool::byteSeqTransformN2B(header.m_nMsgType, headerMsgType);
		memmove(m_pSendData, headerPackLen, sizeof(header.m_nMsgLen));
		memmove(m_pSendData + sizeof(header.m_nMsgLen), headerMsgType, sizeof(header.m_nMsgType));
		memmove(m_pSendData + sizeof(MsgHeader), data, dataSize);
		if(CommonTool::MsgTool::data2Md5(m_pSendData, sizeof(MsgHeader) + dataSize, ender.m_bytesMD5))
		{
			memmove(m_pSendData + sizeof(MsgHeader) + dataSize, (const char*)&ender, sizeof(MsgEnder));
			m_pTcpSoc->write((const char*)m_pSendData, header.m_nMsgLen);
			emit sigSendData(QByteArray((const char*)m_pSendData, header.m_nMsgLen),getUserId());
		}
	};

	if(!isServerBigEndian())		// 服务器小端传输
	{
		// 本机的存储机制
		//if(!CommonTool::MsgTool::isLittleEndian()) // debug
		if(CommonTool::MsgTool::isLittleEndian())	// 本机小端
		{
			lambLocalStoreModeSend();
		}
		else // 本机大端
		{
			lambByteSeqTransformSend();

		}
		
	}
	else // 服务器大端传输
	{
		// 本机的存储机制
		if(CommonTool::MsgTool::isLittleEndian())	// 本机小端
		{
			lambByteSeqTransformSend();
		}
		else
		{
			lambLocalStoreModeSend();
		}
	}
}

void User::onConnected()
{
	emit sigConnect(getUserId());
}

void User::onError(QAbstractSocket::SocketError eCode)
{
	emit sigError(getUserId(), eCode);
}

void User::onReadData()
{
	m_bytesMsg.clear();
	m_bytesMsg = m_pTcpSoc->readAll();

	emit sigReadData(getUserId(), m_bytesMsg);
}

void User::sendHeartInfo()
{
	MsgHeartCS msg;
	memmove(msg.m_bytesHeart, "\x4C\x42\x47\x53", sizeof(msg.m_bytesHeart));
	sendData((const char*)&msg, sizeof(MsgHeartCS), EnMsgType::MSG_TYPE_HEART_CS);
}
