#ifndef __CONNECT_WIDGET_H__
#define __CONNECT_WIDGET_H__

#include "servercommon/basedef.h"
#include <QtWidgets/QWidget>
#include <QtCore/QVector>
#include <QtCore/QRunnable>
#include <QtCore/QObject>

class QLineEdit;
class QPushButton;
class QPlainTextEdit;
class QRadioButton;
class QTimer;
class User;
class QComboBox;

class ConnectWidget : public QWidget
{
	Q_OBJECT

public:
	ConnectWidget(QWidget* parent = NULL);
	~ConnectWidget();

	QString getIpAddress();
	uint getPort();
	uint getConnectCount();
	QPlainTextEdit* getMsgPlantText() { return m_pMsgPlantText; }
	QPlainTextEdit* getLogPlantText() { return m_pLogPlantText; }
	uint& getUserId() { return m_userId; }
	QVector<User*>& getVecUser() { return m_vecUser; }

private:
	void initTimer();
	void initUi();
	bool connectAll();
	void disConnectAll();

private slots:
	void onConnectBtClicked(bool checked);
	void onDisConnectBtClicked(bool checked);
	void onToggled(bool checked);
	void onClearLog(bool checked);
	void onUserConnect(uint userId);
	void onError(uint userId, int eCode);
	void onReadData(uint userId, const QByteArray& data);
	void onSendData(bool checked);
	void onSendData(const QByteArray& data, uint userId);
	void onSendData();
	void onCurrentTextChanged(const QString &str);

private:
	QLineEdit* m_pEditIP;
	QLineEdit* m_pEditPort;
	QLineEdit* m_pEditConnectCount;
	QRadioButton* m_pRadioBt;
	QLineEdit* m_pEditSendInterval;
	QComboBox* m_pCombSendMsgType;
	QPushButton* m_pConnectBt;
	QPushButton* m_pDisConnectBt;
	QPushButton* m_pSendDataBt;
	QPushButton* m_pClearLogBt;
	QPlainTextEdit* m_pMsgPlantText;
	QPlainTextEdit* m_pLogPlantText;
	QTimer* m_pSendDataTimer;

	uint m_userId;
	QVector<User*> m_vecUser;
};

#endif // !__CONNECT_WIDGET_H__
