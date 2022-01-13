#include "user.h"

#include <QtNetwork/QHostAddress>
#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QString>
#include <QtCore/QMetaEnum>

#include <gateserver/gateservermsg/gateservermsg.h>
#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/msgmodule/msgcommondef.h>

User::User(
	const QString& ip,
	uint port,
	QObject* parent
	)
	: m_nUserId(0)
	, m_bServerBigEndian(false)
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

void User::sendData(const char* data, uint dataSize)
{
	if(!m_pTcpSoc)
	{
		return;
	}

	DEFINE_BYTE_ARRAY(sendData, sizeof(MsgHeader) + sizeof(MsgHeartCS) + sizeof(MsgEnder));
	MsgHeader header;
	MsgEnder ender;
	header.m_nPackLen = sizeof(MsgHeader) + sizeof(MsgHeartCS) + sizeof(MsgEnder) - sizeof(header.m_nPackLen);
	header.m_nMsgType = EnMsgType::MSG_TYPE_HEART_CS;
	CommonTool::MsgTool::data2Md5(sendData, sizeof(MsgHeader) + sizeof(MsgHeartCS), ender.m_bytesMD5);

	auto lambLocalStoreModeSend = [&]()
	{
		memmove(sendData, (const char*)&header, sizeof(MsgHeader));
		memmove(sendData + sizeof(MsgHeader), data, dataSize);
		memmove(sendData + sizeof(MsgHeader) + dataSize, (const char*)&ender, sizeof(MsgEnder));

		m_pTcpSoc->write((const char*)sendData, sizeof(MsgHeader) + sizeof(MsgHeartCS) + sizeof(MsgEnder));
	};

	auto lambByteSeqTransformSend = [&]()
	{
		DEFINE_BYTE_ARRAY(headerLittleEndianData, sizeof(MsgHeader) + sizeof(MsgHeartCS) + sizeof(MsgEnder));
		DEFINE_BYTE_ARRAY(headerPackLen, sizeof(header.m_nPackLen));
		DEFINE_BYTE_ARRAY(headerMsgType, sizeof(header.m_nMsgType));

		CommonTool::MsgTool::byteSeqTransformN2B(header.m_nPackLen, headerPackLen);
		CommonTool::MsgTool::byteSeqTransformN2B(header.m_nMsgType, headerMsgType);
		memmove(headerLittleEndianData, headerPackLen, sizeof(header.m_nPackLen));
		memmove(headerLittleEndianData + sizeof(header.m_nPackLen), headerMsgType, sizeof(header.m_nMsgType));
		memmove(headerLittleEndianData + sizeof(MsgHeader), data, dataSize);
		memmove(headerLittleEndianData + sizeof(MsgHeader) + dataSize, (const char*)&ender, sizeof(MsgEnder));

		m_pTcpSoc->write((const char*)headerLittleEndianData, sizeof(MsgHeader) + sizeof(MsgHeartCS) + sizeof(MsgEnder));
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
	sendData((const char*)&msg, sizeof(MsgHeartCS));
}
