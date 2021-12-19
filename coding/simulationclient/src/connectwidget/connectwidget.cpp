#include "connectwidget.h"
#include "user.h"
#include "servercommon/boostmodule/basedef.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QThreadPool>

ConnectWidget::ConnectWidget(QWidget* parent)
	: m_userId(0)
{
	initUi();
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

void ConnectWidget::initUi()
{
	QVBoxLayout* mainVLayout = new QVBoxLayout(this);
	{
		QHBoxLayout* hLayout = new QHBoxLayout(this);
		{
			m_pEditIP = new QLineEdit(this);
			m_pEditPort = new QLineEdit(this);
			m_pEditConnectCount = new QLineEdit(this);
			m_pConnectBt = new QPushButton("connect all", this);
			m_pDisConnectBt = new QPushButton("disconnect", this);
			m_pSendDataBt = new QPushButton("send string data", this);
			m_pClearLogBt = new QPushButton("clear log", this);

			m_pDisConnectBt->setEnabled(false);
			m_pSendDataBt->setEnabled(false);
			m_pEditIP->setPlaceholderText("ip address");
			m_pEditIP->setText("127.0.0.1");
			m_pEditPort->setPlaceholderText("port");
			m_pEditPort->setText("4510");
			m_pEditConnectCount->setPlaceholderText("connect count");
			m_pEditConnectCount->setText("1");

			hLayout->addWidget(m_pEditIP);
			hLayout->addWidget(m_pEditPort);
			hLayout->addWidget(m_pEditConnectCount);
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

void ConnectWidget::onDisConnectBtClicked(bool checked)
{
	disConnectAll();
	m_pConnectBt->setEnabled(true);
	m_pDisConnectBt->setEnabled(false);
	m_pSendDataBt->setEnabled(false);
}

void ConnectWidget::onSendData(bool checked)
{
	for (int i = 0; i < m_vecUser.size(); ++i)
	{
		if (m_vecUser[i])
		{
			m_vecUser[i]->send(m_pMsgPlantText->toPlainText());
		}
	}
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

