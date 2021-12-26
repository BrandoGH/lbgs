#include "connectwidget.h"
#include "user.h"
#include "servercommon/boostmodule/basedef.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QRadioButton>
#include <QtCore/QThreadPool>
#include <QtCore/QTimer>

ConnectWidget::ConnectWidget(QWidget* parent)
	: m_userId(0)
{
	initUi();
	initData();

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

void ConnectWidget::initData()
{
	m_pSendDataTimer = new QTimer(this);
	m_pSendDataTimer->setInterval(m_pEditSendInterval->text().toInt());
	connect(m_pSendDataTimer,SIGNAL(timeout()),this,SLOT(onSendData()));
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

			hLayout->addWidget(m_pEditIP);
			hLayout->addWidget(m_pEditPort);
			hLayout->addWidget(m_pEditConnectCount);
			hLayout->addWidget(m_pRadioBt);
			hLayout->addWidget(m_pEditSendInterval);
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
}

bool ConnectWidget::connectAll()
{
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
		connect(user, SIGNAL(sigReadData(uint, const QString&)), this, SLOT(onReadData(uint, const QString&)));
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
			m_vecUser[i]->send(m_pMsgPlantText->toPlainText());
		}
	}
}

void ConnectWidget::onDisConnectBtClicked(bool checked)
{
	disConnectAll();
	m_pConnectBt->setEnabled(true);
	m_pDisConnectBt->setEnabled(false);
	m_pSendDataBt->setEnabled(false);
}

void ConnectWidget::onToggled(bool checked)
{
	if(!m_pSendDataTimer)
	{
		return;
	}

	if(checked)
	{
		m_pEditSendInterval->setEnabled(false);
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

void ConnectWidget::onClearLog(bool checked)
{
	m_pLogPlantText->clear();
}

void ConnectWidget::onUserConnect(uint userId)
{
	m_pLogPlantText->appendPlainText(
		QString("user[%1] has conneced").arg(userId)
	);
	
	if (m_vecUser[userId - 1])
	{
		m_vecUser[userId - 1]->send(
			QString("user id[%1] msg").arg(m_vecUser[userId - 1]->getUserId())
		);
	}
}

void ConnectWidget::onError(uint userId, int eCode)
{
	m_pLogPlantText->appendPlainText(
		QString("user[%1] has error[%2]")
		.arg(userId)
		.arg(eCode)
	);
}

void ConnectWidget::onReadData(uint userId, const QString & data)
{
	m_pLogPlantText->appendPlainText(
		QString("user[%1] has read data[%2]")
		.arg(userId)
		.arg(data)
	);
}

void ConnectWidget::onConnectBtClicked(bool checked)
{
	if (!connectAll())
	{
		return;
	}

	m_pConnectBt->setEnabled(false);
	m_pDisConnectBt->setEnabled(true);
	m_pSendDataBt->setEnabled(true);
}

