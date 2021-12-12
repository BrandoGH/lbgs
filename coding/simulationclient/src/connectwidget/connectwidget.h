#ifndef __CONNECT_WIDGET_H__
#define __CONNECT_WIDGET_H__

#include "servercommon/basedef.h"
#include <QtWidgets/QWidget>

class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QPlainTextEdit;

class ConnectWidget : public QWidget
{
	Q_OBJECT

public:
	ConnectWidget(QWidget* parent = NULL);
	~ConnectWidget();

	const QString& getIpAddress();
	uint getPort();
	uint getConnectCount();
	QPlainTextEdit* getMsgPlantText() { return m_pMsgPlantText; }
	QPlainTextEdit* getLogPlantText() { return m_pLogPlantText; }

private:
	void initUi();

private slots:
	void onConnectBtClicked(bool checked);

private:
	QVBoxLayout* m_pMainVLayout;
	QHBoxLayout* m_pHLayout;
	QLineEdit* m_pEditIP;
	QLineEdit* m_pEditPort;
	QLineEdit* m_pEditConnectCount;
	QPushButton* m_pConnectBt;
	QPlainTextEdit* m_pMsgPlantText;
	QPlainTextEdit* m_pLogPlantText;
};

#endif // !__CONNECT_WIDGET_H__
