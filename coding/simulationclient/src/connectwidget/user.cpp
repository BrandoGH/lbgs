#include "user.h"
#include "connectwidget.h"

#include <QtNetwork/QHostAddress>
#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QString>
#include <QtCore/QMetaEnum>

User::User(QObject* parent)
{
	m_nUserId = 0;

	m_pConnectWidget = qobject_cast<ConnectWidget*>(parent);
	assert(m_pConnectWidget != NULL);
	m_pTcpSoc = new QTcpSocket(this);
	m_pTcpSoc->connectToHost(
		QHostAddress(QString(m_pConnectWidget->getIpAddress())),
		m_pConnectWidget->getPort());

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
	if (!m_pConnectWidget)
	{
		printf("m_pConnectWidget == NULL\n");
		return;
	}
	m_pConnectWidget->getLogPlantText()
		->appendPlainText(
			QString("user[%1] has connected to [%2 : %3]")
			.arg(getUserId())
			.arg(m_pConnectWidget->getIpAddress())
			.arg(m_pConnectWidget->getPort())
		);

	send(m_pConnectWidget->getMsgPlantText()->toPlainText());
}

void User::onError(QAbstractSocket::SocketError ecode)
{
	if (!m_pConnectWidget)
	{
		printf("m_pConnectWidget == NULL\n");
		return;
	}
	m_pConnectWidget->getLogPlantText()
		->appendPlainText(
			QString("user[%1] catch error: code[%2]")
			.arg(getUserId())
			.arg(ecode)
		);
}

void User::onReadData()
{
	m_strMsg.clear();
	m_strMsg = m_pTcpSoc->readAll();

	if (!m_pConnectWidget)
	{
		printf("m_pConnectWidget == NULL\n");
		return;
	}

	m_pConnectWidget->getLogPlantText()
		->appendPlainText(
			QString("user[%1] read msg[%2]")
			.arg(getUserId())
			.arg(m_strMsg)
		);

}