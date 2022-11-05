#include "mainwindowobj.h"

#include <QtCore/QFile>
#include <QtUiTools/QUiLoader>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtCore/QDebug>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QAction>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QMessageBox>
#include "QtCore/QTimer"
#include "msgmodule/msgcommondef.h"
#include "logicserver/communicationmsg/msgheart.h"
#include "logicserver/communicationmsg/msglogin.h"
#include "commontool/msgtool/msgtool.h"

namespace
{
static int g_nLogLine = 0;
const char* g_strDefaultSendText = "Send text,if send type is not Text,will be show msg structure here";

}

#define SIM_CLIENT_LOG(log) \
if (m_ptLog)\
{\
	m_ptLog->appendPlainText(QString("%1: %2").arg(++g_nLogLine).arg(log));\
}

#define SIM_CLIENT_MSG(text) \
if (m_ptDataText)\
{\
	m_ptDataText->setPlainText(text);\
}


MainWindowObj::MainWindowObj(QObject* parent)
	: QObject(parent)
	, m_nClientId(0)
	, m_mw(NULL)
	, m_lineIp(NULL)
	, m_linePort(NULL)
	, m_lineLinkCount(NULL)
	, m_btConnect(NULL)
	, m_btDisconnect(NULL)
	, m_cbLoopSend(NULL)
	, m_lineSendInterval(NULL)
	, m_combMsgType(NULL)
	, m_btSend(NULL)
	, m_ptDataText(NULL)
	, m_btClearLog(NULL)
	, m_ptLog(NULL)
	, m_pSendDataTimer(NULL)
	, m_bServerLittleStoreMode(false)
	, m_pSendData(NULL)
	, m_acCommunication(NULL)
	, m_acGameLogin(NULL)
	, m_stackWidget(NULL)
	, m_acUIHotReload(NULL)
	, m_lineUserName(NULL)
	, m_linePassword(NULL)
	, m_btLogin(NULL)
{
	loadMainWindow();
	initTimer();

	m_msgSendData.setRawData(g_strDefaultSendText, strlen(g_strDefaultSendText));
}

MainWindowObj::~MainWindowObj()
{
}

void MainWindowObj::show()
{
	if (m_mw)
	{
		m_mw->show();
	}
}

void MainWindowObj::hide()
{
	if (m_mw)
	{
		m_mw->hide();
	}
}

QString MainWindowObj::getIpAddress()
{
	if (!m_lineIp)
	{
		return QString();
	}
	return m_lineIp->text();
}

uint MainWindowObj::getPort()
{
	if (!m_linePort)
	{
		return 0;
	}
	return m_linePort->text().toInt();
}

uint MainWindowObj::getLinkCount()
{
	if (!m_lineLinkCount)
	{
		return 0;
	}
	return m_lineLinkCount->text().toInt();
}

void MainWindowObj::onConnectBtClicked(bool checked)
{
	if (!m_ptLog)
	{
		return;
	}
	if (getLinkCount() <= 0)
	{
		SIM_CLIENT_LOG("link count error!");
		return;
	}

	m_btConnect->setEnabled(false);
	m_btDisconnect->setEnabled(true);

	for (int i = 0; i < getLinkCount(); ++i)
	{
		QSharedPointer<Client> client(new Client(getIpAddress(), getPort(), this));
		client->setClientId(m_nClientId++);
		m_vecClient.push_back(client);

		connect(client.get(), SIGNAL(sigConnect(uint)), this, SLOT(onOnceClientConnected(uint)));
		connect(client.get(), SIGNAL(sigError(uint, int)), this, SLOT(onError(uint, int)));
		connect(client.get(), SIGNAL(sigReadData(uint, const QByteArray&)), this, SLOT(onReadData(uint, const QByteArray&)));
		connect(client.get(), SIGNAL(sigSendData(uint, const QByteArray&)), this, SLOT(onSendData(uint, const QByteArray&)));
		qDebug() << QString("has create client, Id[%1]").arg(client->getClientId());
	}
}


void MainWindowObj::onDisconnectBtClicked(bool checked)
{
	m_nClientId = 0;
	m_vecClient.clear();
	m_btConnect->setEnabled(true);
	m_btDisconnect->setEnabled(false);
	SIM_CLIENT_LOG("all client has disconnect");
}

void MainWindowObj::onClearLog(bool checked)
{
	g_nLogLine = 0;
	if (m_ptLog)
	{
		m_ptLog->clear();
	}
}

void MainWindowObj::onSendClicked(bool checked)
{
	if (m_vecClient.empty())
	{
		SIM_CLIENT_LOG(QString("no client connected!"));
		return;
	}

	for (int i = 0; i < m_vecClient.size(); ++i)
	{
		if (m_vecClient[i])
		{
			m_vecClient[i]->sendData(m_msgSendData);
		}
	}
}

void MainWindowObj::onCurrentIndexChanged(int index)
{
	switch (index)
	{
		case TYPE_TEXT:
		{
			SIM_CLIENT_MSG(g_strDefaultSendText);
			m_msgSendData.setRawData(g_strDefaultSendText, strlen(g_strDefaultSendText));
		}
			break;

		case TYPE_HEART:
		{
			SIM_CLIENT_MSG(R"(
struct MsgHeartCS	// 150-client heart req
{
	MsgHeartCS()
	{
		memset(m_bytesHeart, 0, sizeof(m_bytesHeart));
	}
	byte m_bytesHeart[4];		// LBGS = 0x4C 0x42 0x47 0x53
};
)");
			assembleHeart();
		}
			break;
	}
}

void MainWindowObj::onStateChanged(int status)
{
	if (status == Qt::Checked)
	{
		if (m_lineSendInterval && m_pSendDataTimer && m_lineSendInterval)
		{
			m_lineSendInterval->setEnabled(false);
			bool ok = false;
			int interval_ms = m_lineSendInterval->text().toInt(&ok);
			m_pSendDataTimer->setInterval(ok ? interval_ms : 1000);
			m_pSendDataTimer->start();
		}

	}
	else
	{
		if (m_lineSendInterval && m_pSendDataTimer)
		{
			m_lineSendInterval->setEnabled(true);
			m_pSendDataTimer->stop();
		}
	}
}

void MainWindowObj::onTriggeredCommunication(bool checked)
{
	if (m_stackWidget)
	{
		m_stackWidget->setCurrentIndex(SW_COMMUNICATION);
	}
}

void MainWindowObj::onTriggeredGameLogin(bool checked)
{
	if (m_stackWidget)
	{
		m_stackWidget->setCurrentIndex(SW_GAME_LOGIN);
	}
}

void MainWindowObj::onTriggeredUIHotReload(bool checked)
{
	hide();
	loadMainWindow();
	show();
	qDebug() << "UI Hot Reload Success!!";
}

void MainWindowObj::onLoginClicked(bool checked)
{
	SIM_CLIENT_LOG(QString("waiting......."));
	if (!m_lineUserName || !m_linePassword)
	{
		return;
	}
	if (m_lineUserName->text().isEmpty() || m_linePassword->text().isEmpty())
	{
		QMessageBox::information(m_mw, "error", "please input info", QMessageBox::Ok);
		return;
	}
	assembleLogin();
}

void MainWindowObj::onOnceClientConnected(uint clientId)
{
	SIM_CLIENT_LOG(QString("client[%1] has connected").arg(clientId));
}

void MainWindowObj::onError(uint clientId, int eCode)
{
	SIM_CLIENT_LOG(QString("client[%1] has Error,code[%2]").arg(clientId).arg(eCode));
}

void MainWindowObj::onReadData(uint clientId, const QByteArray& data)
{
	// first link ,server return 
	// 0xAE - LittleEndian  0x01 - BigEndian
	if (data.size() == 1)
	{
		bool ok = false;
		byte value = data[0];
		switch (value)
		{
		case 0xAE:
			SIM_CLIENT_LOG(QString("R_client[%1][%2][%3]: server store mode is littile").arg(clientId).arg(data.size()).arg(QString(data.toHex().toUpper())));
			m_bServerLittleStoreMode = true;
			break;
		case 0x01:
			SIM_CLIENT_LOG(QString("R_client[%1][%2][%3]: server store mode is big").arg(clientId).arg(data.size()).arg(QString(data.toHex().toUpper())));
			break;
		default:
			SIM_CLIENT_LOG(QString("R_client[%1][%2]: server store mode unknown!!!, socket will be close").arg(clientId).arg(data.size()));
			m_vecClient[clientId]->closeSocket();
			break;
		}
		return;
	}
	SIM_CLIENT_LOG(QString("R_client[%1][%2][%3]").arg(clientId).arg(data.size()).arg(QString(data.toHex().toUpper())));

	MsgHeader* header = (MsgHeader*)(data.data());
	if (!header)
	{
		return;
	}
	switch (header->m_nMsgType)
	{
	case MSG_TYPE_LOGIN_SC:
		onLoginMsg(data);
		break;
	}
}

void MainWindowObj::onLoginMsg(const QByteArray& data)
{
	MsgLoginSC* sc = (MsgLoginSC*)(data.data() + sizeof(MsgHeader));
	if (!sc)
	{
		return;
	}

	QString strTipInfo;

	if (sc->m_cLoginStatus == MsgLoginSC::LS_LOGIN_OK && 
		sc->m_cErrorReason == MsgLoginSC::ER_NO_ERROR)
	{
		// into game world TODO

		strTipInfo = "login ok!!";
		QMessageBox::information(m_mw, "login", strTipInfo);
		return;
	}

	if (sc->m_cLoginStatus == MsgLoginSC::LS_LOGIN_ERROR)
	{
		switch (sc->m_cErrorReason)
		{
		case MsgLoginSC::ER_PASSWORD_ERROR:
			strTipInfo = "password error";
			break;
		case MsgLoginSC::ER_HAS_LOGIN_ERROR:
			strTipInfo = "this role has been logined!!";
			break;
		case MsgLoginSC::ER_RELOGIN_ERROR:
			strTipInfo = "last time logout error, please login again!!";
			break;
		}

		QMessageBox::information(m_mw, "error", strTipInfo);
	}
}


void MainWindowObj::onSendData(uint clientId, const QByteArray& data)
{
	SIM_CLIENT_LOG(QString("S_client[%1][%2][%3]").arg(clientId).arg(data.size()).arg(QString(data.toHex().toUpper())));
}

void MainWindowObj::loadMainWindow()
{
	QUiLoader uiLoader;
	QFile uiFile;
	if (!loadUiFile(&uiFile, "mainwindow.ui"))
	{
		return;
	}
	uiFile.open(QFile::ReadOnly);
	m_mw = (QMainWindow*)uiLoader.load(&uiFile);
	uiFile.close();

	if (m_mw)
	{
		// Communication
		m_lineIp = m_mw->findChild<QLineEdit*>("lineEdit_ip");
		m_linePort = m_mw->findChild<QLineEdit*>("lineEdit_port");
		m_lineLinkCount = m_mw->findChild<QLineEdit*>("lineEdit_linkCount");
		m_btConnect = m_mw->findChild<QPushButton*>("pushButton_connect");
		m_btDisconnect = m_mw->findChild<QPushButton*>("pushButton_disconnect");
		m_cbLoopSend = m_mw->findChild<QCheckBox*>("checkBox_loopSendData");
		m_lineSendInterval = m_mw->findChild<QLineEdit*>("lineEdit_sendDataInterval");
		m_combMsgType = m_mw->findChild<QComboBox*>("comboBox_msgType");
		m_btSend = m_mw->findChild<QPushButton*>("pushButton_sendData");
		m_ptDataText = m_mw->findChild<QPlainTextEdit*>("plainTextEdit_msgData");
		m_btClearLog = m_mw->findChild<QPushButton*>("pushButton_clearLog");
		m_ptLog = m_mw->findChild<QPlainTextEdit*>("plainTextEdit_log");
		m_stackWidget = m_mw->findChild<QStackedWidget*>("stackedWidget");
		m_acCommunication = m_mw->findChild<QAction*>("actionCommunication");
		m_acGameLogin = m_mw->findChild<QAction*>("actionGameLogin");
		m_acUIHotReload = m_mw->findChild<QAction*>("actionUIHotReload");

		// Game login simulation
		m_lineUserName = m_mw->findChild<QLineEdit*>("lineEdit_loginName");
		m_linePassword = m_mw->findChild<QLineEdit*>("lineEdit_loginPassword");
		m_btLogin = m_mw->findChild<QPushButton*>("pushButton_login");



		if (m_btConnect && m_btDisconnect)
		{
			m_btConnect->setEnabled(true);
			m_btDisconnect->setEnabled(false);
		}
		// Communication
		connect(m_btConnect, SIGNAL(clicked(bool)), this, SLOT(onConnectBtClicked(bool)));
		connect(m_btDisconnect, SIGNAL(clicked(bool)), this, SLOT(onDisconnectBtClicked(bool)));
		connect(m_btClearLog, SIGNAL(clicked(bool)), this, SLOT(onClearLog(bool)));
		connect(m_btSend, SIGNAL(clicked(bool)), this, SLOT(onSendClicked(bool)));
		connect(m_combMsgType, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onCurrentIndexChanged(int)));
		connect(m_cbLoopSend, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
		connect(m_acCommunication, SIGNAL(triggered(bool)), this, SLOT(onTriggeredCommunication(bool)));
		connect(m_acGameLogin, SIGNAL(triggered(bool)), this, SLOT(onTriggeredGameLogin(bool)));
		connect(m_acUIHotReload, SIGNAL(triggered(bool)), this, SLOT(onTriggeredUIHotReload(bool)));

		// Game login simulation
		connect(m_btLogin, SIGNAL(clicked(bool)), this, SLOT(onLoginClicked(bool)));

	}

	if (m_stackWidget)
	{
		m_stackWidget->setCurrentIndex(SW_COMMUNICATION);
	}
	
}

bool MainWindowObj::loadUiFile(QFile* file, const QString& fileName)
{
	if (!file)
	{
		return false;
	}
	file->setFileName(UI_FILE_DIR_EXE + fileName);
	if (!file->exists())
	{
		file->setFileName(UI_FILE_DIR_SRC + fileName);
	}
	if (!file->exists())
	{
		qDebug() << "loadMainWindow() error";
		return false;
	}
	return true;
}

void MainWindowObj::initTimer()
{
	m_pSendDataTimer = new QTimer(this);
	if (m_lineSendInterval)
	{
		m_pSendDataTimer->setInterval(m_lineSendInterval->text().toInt());
	}
	connect(m_pSendDataTimer, &QTimer::timeout, this,
		[&]()
	{
		assembleHeart();
		onSendClicked(true);
	});
}

void MainWindowObj::assembleProtocal(const char* bodyData, uint dataSize, int msgType, QByteArray& outData)
{
	int msgLen = sizeof(MsgHeader) + dataSize + sizeof(MsgEnder);

	// hearder
	MsgHeader header;
	header.m_nMsgLen = msgLen;
	header.m_nMsgType = msgType;

	// body
	DEL_ARR(m_pSendData);
	m_pSendData = new byte[header.m_nMsgLen];

	// ender
	MsgEnder ender;
	auto lambLocalStoreLittleMode = [&]()
	{
		memmove(m_pSendData, (const char*)&header, sizeof(MsgHeader));
		memmove(m_pSendData + sizeof(MsgHeader), bodyData, dataSize);
		if (CommonTool::MsgTool::data2Md5(m_pSendData, sizeof(MsgHeader) + dataSize, ender.m_bytesMD5))
		{
			memmove(m_pSendData + sizeof(MsgHeader) + dataSize, (const char*)&ender, sizeof(MsgEnder));

			// test to server
			static bool bTCPStickyPacketsSwitch = true;
			static bool bRunTCPStickyPacketsTest = false; // true/false
			if (!bRunTCPStickyPacketsTest)
			{
				outData.setRawData((const char*)m_pSendData, msgLen);
			}
			else
			{
				/*
				* [msg1][msg2:header+...]
				* [...][msg3]
				*/
				int mode1 = sizeof(MsgHeader) + 10;

				/*
				* [msg1][msg2:hea]
				* [der....][msg3]
				*/
				int mode2 = sizeof(MsgHeader) - 10;
				if (bTCPStickyPacketsSwitch)
				{
					
					outData.setRawData((const char*)m_pSendData, msgLen);
					outData.append((const char*)m_pSendData, mode1);
					bTCPStickyPacketsSwitch = false;
				} else
				{
					// 
					outData.setRawData((const char*)m_pSendData + mode1, msgLen - mode1);
					outData.append((const char*)m_pSendData, msgLen);
					outData.append((const char*)m_pSendData, msgLen);
					bTCPStickyPacketsSwitch = true;
				}
				SIM_CLIENT_LOG(QString("bTCPStickyPacketsSwitch[%1]").arg(bTCPStickyPacketsSwitch));
			}
			
			
		}
	};

	// only suport little store
	if (isServerLittleStore() && CommonTool::MsgTool::isLittleEndian())
	{
		lambLocalStoreLittleMode();
	}
}

void MainWindowObj::assembleHeart()
{
	MsgHeartCS msg;
	memmove(msg.m_bytesHeart, I_MSG_HEART_CS, sizeof(msg.m_bytesHeart));
	assembleProtocal((const char*)&msg, sizeof(MsgHeartCS), EnMsgType::MSG_TYPE_HEART_CS, m_msgSendData);

}

void MainWindowObj::assembleLogin()
{
	if (!m_lineUserName || !m_linePassword)
	{
		return;
	}
	MsgLoginCS msg;
	memmove(msg.m_strRoleName, m_lineUserName->text().toStdString().data(), sizeof(msg.m_strRoleName));
	memmove(msg.m_strPassword, m_linePassword->text().toStdString().data(), sizeof(msg.m_strPassword));
	QByteArray data;
	assembleProtocal((const char*)&msg, sizeof(MsgLoginCS), EnMsgType::MSG_TYPE_LOGIN_CS, data);
	for (int i = 0; i < m_vecClient.size(); ++i)
	{
		if (m_vecClient[i])
		{
			m_vecClient[i]->sendData(data);
		}
	}
}
