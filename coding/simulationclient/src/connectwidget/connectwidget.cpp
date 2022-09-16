#include "connectwidget.h"
#include "user.h"
#include "servercommon/boostmodule/basedef.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QtCore/QThreadPool>
#include <QtCore/QTimer>

ConnectWidget::ConnectWidget(QWidget* parent)
	: m_userId(0)
{
	initUi();
	initTimer();

}

ConnectWidget::~ConnectWidget()
{
}

QString ConnectWidget::getIpAddress()
{
	if (!m_pEditIP)
	{
		return QString("");
	}
	return m_pEditIP->text();
}

uint ConnectWidget::getPort()
{
	if (!m_pEditPort)
	{
		return 0;
	}
	return m_pEditPort->text().toUInt();
}

uint ConnectWidget::getConnectCount()
{
	if (!m_pEditConnectCount)
	{
		return 0;
	}
	return m_pEditConnectCount->text().toUInt();
}

void ConnectWidget::initTimer()
{
	m_pSendDataTimer = new QTimer(this);
	m_pSendDataTimer->setInterval(m_pEditSendInterval->text().toInt());
	connect(m_pSendDataTimer, SIGNAL(timeout()), this, SLOT(onSendData()));
}

void ConnectWidget::initUi()
{
	QVBoxLayout* mainVLayout = new QVBoxLayout(this);
	{
		QHBoxLayout* hLayout = new QHBoxLayout(this);
		{
			m_pEditIP = new QLineEdit(this);
			m_pEditPort = new QLineEdit(this);
			m_pEditConnectCount = new QLineEdit(this);
			m_pRadioBt = new QRadioButton("loop send data", this);
			m_pConnectBt = new QPushButton("connect all", this);
			m_pDisConnectBt = new QPushButton("disconnect", this);
			m_pSendDataBt = new QPushButton("send string data", this);
			m_pClearLogBt = new QPushButton("clear log", this);
			m_pEditSendInterval = new QLineEdit(this);
			m_pCombSendMsgType = new QComboBox(this);

			m_pDisConnectBt->setEnabled(false);
			m_pSendDataBt->setEnabled(false);
			m_pEditIP->setPlaceholderText("ip address");
			m_pEditIP->setText("127.0.0.1");
			m_pEditPort->setPlaceholderText("port");
			m_pEditPort->setText("4510");
			m_pEditConnectCount->setPlaceholderText("connect count");
			m_pEditConnectCount->setText("1");
			m_pRadioBt->setChecked(false);
			m_pEditSendInterval->setPlaceholderText("send data interval (msec)");
			m_pEditSendInterval->setText("3000");
			m_pCombSendMsgType->addItem("test msg");
			m_pCombSendMsgType->addItem("heart");

			hLayout->addWidget(m_pEditIP);
			hLayout->addWidget(m_pEditPort);
			hLayout->addWidget(m_pEditConnectCount);
			hLayout->addWidget(m_pRadioBt);
			hLayout->addWidget(m_pEditSendInterval);
			hLayout->addWidget(m_pCombSendMsgType);
			hLayout->addWidget(m_pConnectBt);
			hLayout->addWidget(m_pDisConnectBt);
			hLayout->addWidget(m_pSendDataBt);
			hLayout->addWidget(m_pClearLogBt);
		}
		

		m_pMsgPlantText = new QPlainTextEdit(this);
		m_pMsgPlantText->setPlainText("test msg");
		m_pLogPlantText = new QPlainTextEdit(this);
		m_pLogPlantText->setReadOnly(true);

		mainVLayout->addLayout(hLayout);
		mainVLayout->addWidget(m_pMsgPlantText);
		mainVLayout->addWidget(m_pLogPlantText);
	}
	

	setLayout(mainVLayout);

	connect(m_pConnectBt, SIGNAL(clicked(bool)), this, SLOT(onConnectBtClicked(bool)));
	connect(m_pDisConnectBt, SIGNAL(clicked(bool)), this, SLOT(onDisConnectBtClicked(bool)));
	connect(m_pRadioBt, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
	connect(m_pSendDataBt, SIGNAL(clicked(bool)), this, SLOT(onSendData(bool)));
	connect(m_pClearLogBt, SIGNAL(clicked(bool)), this, SLOT(onClearLog(bool)));
	connect(m_pCombSendMsgType, SIGNAL(currentTextChanged(const QString&)), 
		this, SLOT(onCurrentTextChanged(const QString&)));
}

bool ConnectWidget::connectAll()
{
	if(!m_pLogPlantText)
	{
		return false;
	}
	if (getConnectCount() <= 0)
	{
		m_pLogPlantText->appendPlainText("type connect cout error");
		return false;
	}

	for (int i = 0; i < getConnectCount(); ++i)
	{
		User* user = new User(getIpAddress(), getPort(), this);
		user->setUserId(++m_userId);
		m_vecUser.push_back(user);

		connect(user, SIGNAL(sigConnect(uint)), this, SLOT(onUserConnect(uint)));
		connect(user, SIGNAL(sigError(uint, int)), this, SLOT(onError(uint, int)));
		connect(user, SIGNAL(sigReadData(uint, const QByteArray&)), this, SLOT(onReadData(uint, const QByteArray&)));
		connect(user, SIGNAL(sigSendData(const QByteArray&, uint)), this, SLOT(onSendData(const QByteArray&, uint)));
		printf("has create user [%d]\n", i + 1);
	}

	return true;
}

void ConnectWidget::disConnectAll()
{
	for (int i = 0; i < m_vecUser.size(); ++i)
	{
		if (m_vecUser[i])
		{
			delete m_vecUser[i];
			m_vecUser[i] = NULL;
		}
	}

	m_userId = 0;
	m_vecUser.clear();
}

void ConnectWidget::onSendData()
{
	for(int i = 0; i < m_vecUser.size(); ++i)
	{
		if(m_vecUser[i])
		{
			m_vecUser[i]->sendData(m_pMsgPlantText->toPlainText().toUtf8());
		}
	}
}

void ConnectWidget::onCurrentTextChanged(const QString& str)
{
	if(m_pMsgPlantText)
	{
		m_pMsgPlantText->setPlainText(str);
	}
}

void ConnectWidget::onDisConnectBtClicked(bool checked)
{
	if(!m_pConnectBt || !m_pDisConnectBt || !m_pSendDataBt)
	{
		return;
	}
	disConnectAll();
	m_pConnectBt->setEnabled(true);
	m_pDisConnectBt->setEnabled(false);
	m_pSendDataBt->setEnabled(false);
}

void ConnectWidget::onToggled(bool checked)
{
	if(!m_pSendDataTimer || !m_pEditSendInterval)
	{
		return;
	}

	if(checked)
	{
		m_pEditSendInterval->setEnabled(false);
		m_pSendDataTimer->setInterval(m_pEditSendInterval->text().toInt());
		m_pSendDataTimer->start();
		return;
	}

	m_pEditSendInterval->setEnabled(true);
	m_pSendDataTimer->stop();

}

void ConnectWidget::onSendData(bool checked)
{
	onSendData();
}

void ConnectWidget::onSendData(const QByteArray& data, uint userId)
{
	if(!m_pLogPlantText)
	{
		return;
	}
	QString text;
	// OUPUT BYTE
	/*for(int i = 0; i < data.size(); ++i)
	{
		QByteArray hex(1,data[i]);
		text.append(QString("[%1]0x" + hex.toHex().toUpper()).arg(i));
	}*/
	m_pLogPlantText->appendPlainText(QString("S_%1[%2]: %3").arg(userId).arg(data.size()).arg(QString(data.toHex())));
}

void ConnectWidget::onClearLog(bool checked)
{
	if(m_pLogPlantText)
	{
		m_pLogPlantText->clear();
	}
}

void ConnectWidget::onUserConnect(uint userId)
{
	if(m_pLogPlantText)
	{
		m_pLogPlantText->appendPlainText(
			QString("user[%1] has conneced").arg(userId)
		);
	}
}

void ConnectWidget::onError(uint userId, int eCode)
{
	if(m_pLogPlantText)
	{
		m_pLogPlantText->appendPlainText(
			QString("user[%1] has error[%2]")
			.arg(userId)
			.arg(eCode)
		);
	}
}

void ConnectWidget::onReadData(uint userId, const QByteArray& data)
{
	User* user = m_vecUser[userId - 1];
	if(!m_pLogPlantText || !user)
	{
		return;
	}

	if(data.size() == 1)		// first link ,server return
	{
		if(data.toUInt() == 1)
		{
			m_pLogPlantText->appendPlainText("server store mode is big");
			user->setBigEndian(true);
		}
		else
		{
			m_pLogPlantText->appendPlainText("server store mode is littile");
			user->setBigEndian(false);
		}
		return;
	}
	m_pLogPlantText->appendPlainText(QString("R_%1[%2]: %3").arg(userId).arg(data.size()).arg(QString(data.toHex())));
}

void ConnectWidget::onConnectBtClicked(bool checked)
{
	if (!connectAll() || !m_pConnectBt || !m_pDisConnectBt || !m_pSendDataBt)
	{
		return;
	}

	m_pConnectBt->setEnabled(false);
	m_pDisConnectBt->setEnabled(true);
	m_pSendDataBt->setEnabled(true);
}

