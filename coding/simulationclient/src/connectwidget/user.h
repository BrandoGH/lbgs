#ifndef __USER_H__
#define __USER_H__

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

class ConnectWidget;
class QPlainTextEdit;

class User : public QObject
{
	Q_OBJECT

public:
	User(QObject* parent = NULL);
	~User();
	const QString& getReadData() { return m_strMsg; }
	void setUserId(uint id) { m_nUserId = id; }
	uint getUserId() { return m_nUserId; }

	void send(const QString& msg);

private slots:
	void onConnected();
	void onError(QAbstractSocket::SocketError ecode);
	void onReadData();

private:
	ConnectWidget* m_pConnectWidget;
	QTcpSocket* m_pTcpSoc;
	QString m_strMsg;
	uint m_nUserId;
};

#endif // !__USER_H__
