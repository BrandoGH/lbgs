#include "client.h"

#include <QtNetwork/QHostAddress>
#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QString>
#include <QtCore/QMetaEnum>

#include <servercommon/commontool/commontool.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/msgmodule/msgcommondef.h>
#include <logicserver/communicationmsg/msgheart.h>
#include "QtNetwork/QAbstractSocket"

Client::Client(
	const QString& ip,
	uint port,
	QObject* parent
)
	: QObject(parent)
{
	m_pTcpSoc = new QTcpSocket(this);
	m_pTcpSoc->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption, 1);
	m_pTcpSoc->setSocketOption(QAbstractSocket::SocketOption::KeepAliveOption, 1);

	m_pTcpSoc->connectToHost(
		QHostAddress(ip),
		port);

	connect(m_pTcpSoc, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(m_pTcpSoc, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(m_pTcpSoc, SIGNAL(readyRead()), this, SLOT(onReadData()));
}

Client::~Client()
{
	closeSocket();
}

void Client::sendData(const QByteArray& msg)
{
	if (!m_pTcpSoc)
	{
		return;
	}
	m_pTcpSoc->write(msg);
	emit sigSendData(getClientId(), msg);
}

void Client::closeSocket()
{
	if (m_pTcpSoc)
	{
		m_pTcpSoc->close();
	}
}

void Client::onConnected()
{
	emit sigConnect(getClientId());
}

void Client::onError(QAbstractSocket::SocketError eCode)
{
	emit sigError(getClientId(), eCode);
	closeSocket();
}

void Client::onReadData()
{
	if (!m_pTcpSoc)
	{
		return;
	}
	m_bytesMsg.clear();
	m_bytesMsg = m_pTcpSoc->readAll();

	emit sigReadData(getClientId(), m_bytesMsg);
}
