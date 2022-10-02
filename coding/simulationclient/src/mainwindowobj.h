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
class QAction;
class QStackedWidget;
class QFile;

class MainWindowObj : public QObject
{
	Q_OBJECT

public:
	enum EnMsgTypeCombo
	{
		TYPE_TEXT,
		TYPE_HEART,
	};

	enum EnStackWidgetIndex
	{
		SW_COMMUNICATION,
		SW_GAME_LOGIN,
	};

public:
	MainWindowObj(QObject* parent = NULL);
	~MainWindowObj();

	void show();
	void hide();

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
	void onTriggeredCommunication(bool checked);
	void onTriggeredGameLogin(bool checked);
	void onTriggeredUIHotReload(bool checked);
	void onLoginClicked(bool checked);

	// client
	void onOnceClientConnected(uint clientId);
	void onError(uint clientId, int eCode);
	void onReadData(uint clientId, const QByteArray& data);
	void onSendData(uint clientId, const QByteArray& data);

HANDLER:
	void onLoginMsg(const QByteArray& data);

private:
	void loadMainWindow();
	bool loadUiFile(QFile* file, const QString& fileName);
	void initTimer();
	bool isServerLittleStore() { return m_bServerLittleStoreMode; }

	// protocol
	void assembleProtocal(const char* bodyData, uint dataSize, int msgType);
	void assembleHeart();
	void assembleLogin();

private:
	QMainWindow* m_mw;

	// Communication
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

	QStackedWidget* m_stackWidget;
	QAction* m_acCommunication;
	QAction* m_acGameLogin;
	QAction* m_acUIHotReload;

	// Game login simulation
	QLineEdit* m_lineUserName;
	QLineEdit* m_linePassword;
	QPushButton* m_btLogin;



	QVector<QSharedPointer<Client>> m_vecClient;
	uint m_nClientId;
	QByteArray m_msgSendData;
	QTimer* m_pSendDataTimer;

	bool m_bServerLittleStoreMode;
	byte* m_pSendData;


};

#endif // !__MAIN_WINDOW_OBJ_H_
