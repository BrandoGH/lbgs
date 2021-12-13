#ifndef __USER_H__
#define __USER_H__

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

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
	const QString& getReadData() { return m_strMsg; }
	void setUserId(uint id) { m_nUserId = id; }
	uint getUserId() { return m_nUserId; }

	void send(const QString& msg);

signals:
	void sigConnect(uint userId);
	void sigError(uint userId, int eCode);
	void sigReadData(uint userId, const QString& data);

private slots:
	void onConnected();
	void onError(QAbstractSocket::SocketError eCode);
	void onReadData();

private:
	QTcpSocket* m_pTcpSoc;
	QString m_strMsg;
	uint m_nUserId;
};

#endif // !__USER_H__
