#ifndef __MAIN_WINDOW_OBJ_H_
#define __MAIN_WINDOW_OBJ_H_

#include <QtCore/QObject>
#include "client.h"

class QMainWindow;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QPlainTextEdit;
class QTimer;

class MainWindowObj : public QObject
{
	Q_OBJECT

public:
	enum EnMsgTypeCombo
	{
		TYPE_TEXT,
		TYPE_HEART,
	};

public:
	MainWindowObj(QObject* parent = NULL);
	~MainWindowObj();

	void show();

	QString getIpAddress();
	uint getPort();
	uint getLinkCount();

private slots:
	void onConnectBtClicked(bool checked);
	void onDisconnectBtClicked(bool checked);
	void onClearLog(bool checked);
	void onSendClicked(bool checked);
	void onCurrentIndexChanged(int index);
	void onStateChanged(int status);

	// client
	void onOnceClientConnected(uint clientId);
	void onError(uint clientId, int eCode);
	void onReadData(uint clientId, const QByteArray& data);
	void onSendData(uint clientId, const QByteArray& data);

private:
	void loadMainWindow();
	void initTimer();
	bool isServerLittleStore() { return m_bServerLittleStoreMode; }

	// protocol
	void assembleProtocal(const char* bodyData, uint dataSize, int msgType);
	void assembleHeart();

private:
	QMainWindow* m_mw;
	QLineEdit* m_lineIp;
	QLineEdit* m_linePort;
	QLineEdit* m_lineLinkCount;
	QPushButton* m_btConnect;
	QPushButton* m_btDisconnect;

	QCheckBox* m_cbLoopSend;
	QLineEdit* m_lineSendInterval;

	QComboBox* m_combMsgType;
	QPushButton* m_btSend;
	QPlainTextEdit* m_ptDataText;

	QPushButton* m_btClearLog;
	QPlainTextEdit* m_ptLog;

	QVector<QSharedPointer<Client>> m_vecClient;
	uint m_nClientId;
	QByteArray m_msgSendData;
	QTimer* m_pSendDataTimer;

	bool m_bServerLittleStoreMode;
	byte* m_pSendData;


};

#endif // !__MAIN_WINDOW_OBJ_H_
