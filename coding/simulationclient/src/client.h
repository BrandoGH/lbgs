#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <servercommon/basedef.h>
#include "QtCore/QSharedPointer"

class QPlainTextEdit;

class Client : public QObject, public QEnableSharedFromThis<Client>
{
	Q_OBJECT

public:
	Client(
		const QString& ip,
		uint port, 
		QObject* parent = NULL
	);
	~Client();

	void sendData(const QByteArray& msg);
	void setClientId(uint id) { m_nClientId = id; }
	uint getClientId() { return m_nClientId; }
	void closeSocket();

signals:
	void sigConnect(uint clientId);
	void sigError(uint clientId, int eCode);
	void sigReadData(uint clientId, const QByteArray& data);
	void sigSendData(uint clientId, const QByteArray& data);

private slots:
	void onConnected();
	void onError(QAbstractSocket::SocketError eCode);
	void onReadData();

private:
	QTcpSocket* m_pTcpSoc;
	QByteArray m_bytesMsg;
	uint m_nClientId;
};

#endif // !__CLIENT_H__
