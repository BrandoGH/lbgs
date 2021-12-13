#include "user.h"

#include <QtNetwork/QHostAddress>
#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QString>
#include <QtCore/QMetaEnum>

User::User(
	const QString& ip,
	uint port,
	QObject* parent
	)
{
	m_nUserId = 0;

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
	m_pTcpSoc->close();
}

void User::send(const QString & msg)
{
	m_pTcpSoc->write(msg.toStdString().data(),msg.size());
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
	m_strMsg.clear();
	m_strMsg = m_pTcpSoc->readAll();

	emit sigReadData(getUserId(), m_strMsg);
}