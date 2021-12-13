#ifndef __CONNECT_WIDGET_H__
#define __CONNECT_WIDGET_H__

#include "servercommon/basedef.h"
#include <QtWidgets/QWidget>
#include <QtCore/QVector>
#include <QtCore/QRunnable>
#include <QtCore/QObject>

class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QPlainTextEdit;
class User;

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
	void initUi();
	bool connectAll();
	void disConnectAll();

private slots:
	void onConnectBtClicked(bool checked);
	void onDisConnectBtClicked(bool checked);
	void onSendData(bool checked);
	void onClearLog(bool checked);
	void onUserConnect(uint userId);
	void onError(uint userId, int eCode);
	void onReadData(uint userId, const QString& data);

private:
	QVBoxLayout* m_pMainVLayout;
	QHBoxLayout* m_pHLayout;
	QLineEdit* m_pEditIP;
	QLineEdit* m_pEditPort;
	QLineEdit* m_pEditConnectCount;
	QPushButton* m_pConnectBt;
	QPushButton* m_pDisConnectBt;
	QPushButton* m_pSendDataBt;
	QPushButton* m_pClearLogBt;
	QPlainTextEdit* m_pMsgPlantText;
	QPlainTextEdit* m_pLogPlantText;

	uint m_userId;
	QVector<User*> m_vecUser;
};

#endif // !__CONNECT_WIDGET_H__
