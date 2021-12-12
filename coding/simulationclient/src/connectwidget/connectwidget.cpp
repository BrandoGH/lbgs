#include "connectwidget.h"
#include "user.h"

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>

ConnectWidget::ConnectWidget(QWidget* parent)
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
	m_pMainVLayout = new QVBoxLayout(this);
	{
		m_pHLayout = new QHBoxLayout(this);
		{
			m_pEditIP = new QLineEdit(this);
			m_pEditPort = new QLineEdit(this);
			m_pEditConnectCount = new QLineEdit(this);
			m_pConnectBt = new QPushButton("connect", this);
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

		m_pMsgPlantText = new QPlainTextEdit(this);
		m_pLogPlantText = new QPlainTextEdit(this);
		m_pLogPlantText->setReadOnly(true);
	}
	

	m_pMainVLayout->addLayout(m_pHLayout);
	m_pMainVLayout->addWidget(m_pMsgPlantText);
	m_pMainVLayout->addWidget(m_pLogPlantText);

	setLayout(m_pMainVLayout);

	connect(m_pConnectBt, SIGNAL(clicked(bool)), this, SLOT(onConnectBtClicked(bool)));
}

void ConnectWidget::onConnectBtClicked(bool checked)
{
	if (getConnectCount() <= 0)
	{
		m_pLogPlantText->appendPlainText("type connect cout error");
	}

	for (int i = 0; i < getConnectCount(); ++i)
	{
		User* user = new User(this);
	}
}
