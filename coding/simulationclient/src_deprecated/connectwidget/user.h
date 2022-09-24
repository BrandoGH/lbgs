#ifndef __USER_H__
#define __USER_H__

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <servercommon/basedef.h>

class QPlainTextEdit;

class User : public QObject
{
	Q_OBJECT

public:
	User(
		const QString& ip,
		uint port,
		QObject* parent = NULL
		);
	~User();
	const QByteArray& getReadData() { return m_bytesMsg; }
	void setUserId(uint id) { m_nUserId = id; }
	uint getUserId() { return m_nUserId; }

	// Before passing in data, convert the byte order problem before passing it
	void sendData(const char* data, uint dataSize, int msgType);
	void sendData(const QByteArray& msg);
	void setBigEndian(bool b) { m_bServerBigEndian = b; }
	bool isServerBigEndian() { return m_bServerBigEndian; }

signals:
	void sigConnect(uint userId);
	void sigError(uint userId, int eCode);
	void sigReadData(uint userId, const QByteArray& data);
	void sigSendData(const QByteArray& data, uint sendUserId);

private slots:
	void onConnected();
	void onError(QAbstractSocket::SocketError eCode);
	void onReadData();

private:
	void sendHeartInfo();

private:
	QTcpSocket* m_pTcpSoc;
	QByteArray m_bytesMsg;
	uint m_nUserId;
	bool m_bServerBigEndian;		// server byte order, false-little true-big
	byte* m_pSendData;
};

#endif // !__USER_H__
