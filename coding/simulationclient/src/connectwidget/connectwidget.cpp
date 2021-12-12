#include "connectwidget.h"
#include "user.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>

ConnectWidget::ConnectWidget(QWidget* parent)
	: m_userId(0)
{
	initUi();
}

ConnectWidget::~ConnectWidget()
{
}

const QString& ConnectWidget::getIpAddress()
{
	if (!m_pEditIP)
	{
		return QString();
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
	m_pMainVLayout = new QVBoxLayout(this);
	{
		m_pHLayout = new QHBoxLayout(this);
		{
			m_pEditIP = new QLineEdit(this);
			m_pEditPort = new QLineEdit(this);
			m_pEditConnectCount = new QLineEdit(this);
			m_pConnectBt = new QPushButton("connect all", this);
			m_pDisConnectBt = new QPushButton("disconnect", this);
			m_pDisConnectBt->setEnabled(false);
			m_pEditIP->setPlaceholderText("ip address");
			m_pEditIP->setText("127.0.0.1");
			m_pEditPort->setPlaceholderText("port");
			m_pEditPort->setText("4510");
			m_pEditConnectCount->setPlaceholderText("connect count");
			m_pEditConnectCount->setText("1");
		}
		m_pHLayout->addWidget(m_pEditIP);
		m_pHLayout->addWidget(m_pEditPort);
		m_pHLayout->addWidget(m_pEditConnectCount);
		m_pHLayout->addWidget(m_pConnectBt);
		m_pHLayout->addWidget(m_pDisConnectBt);

		m_pMsgPlantText = new QPlainTextEdit(this);
		m_pMsgPlantText->setPlainText("test msg");
		m_pLogPlantText = new QPlainTextEdit(this);
		m_pLogPlantText->setReadOnly(true);
	}
	

	m_pMainVLayout->addLayout(m_pHLayout);
	m_pMainVLayout->addWidget(m_pMsgPlantText);
	m_pMainVLayout->addWidget(m_pLogPlantText);

	setLayout(m_pMainVLayout);

	connect(m_pConnectBt, SIGNAL(clicked(bool)), this, SLOT(onConnectBtClicked(bool)));
	connect(m_pDisConnectBt, SIGNAL(clicked(bool)), this, SLOT(onDisConnectBtClicked(bool)));
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
		User* user = new User(this);
		user->setUserId(++m_userId);
		m_vecUser.push_back(user);
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

	m_vecUser.clear();
}

void ConnectWidget::onDisConnectBtClicked(bool checked)
{
	disConnectAll();
	m_pConnectBt->setEnabled(true);
	m_pDisConnectBt->setEnabled(false);
}

void ConnectWidget::onConnectBtClicked(bool checked)
{
	if (!connectAll())
	{
		return;
	}

	m_pConnectBt->setEnabled(false);
	m_pDisConnectBt->setEnabled(true);
}
