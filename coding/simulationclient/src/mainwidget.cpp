#include "mainwidget.h"
#include "connectwidget/connectwidget.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

MainWidget::MainWidget(QMainWindow* parent)
{
	setGeometry(100, 100, 1500, 800);
	initMenuBar();
	initCentralWidget();
	
}

MainWidget::~MainWidget()
{
}

void MainWidget::initMenuBar()
{
	m_pMenuBar = new QMenuBar(this);
	m_pMenu = new QMenu("test option", this);
	m_pAcConnectTest = new QAction(QIcon(), "connect test", this);

	m_pMenu->addAction(m_pAcConnectTest);
	m_pMenuBar->addMenu(m_pMenu);
	setMenuBar(m_pMenuBar);

	connect(m_pAcConnectTest, SIGNAL(triggered(bool)), 
		this, SLOT(onConnectTestTriggered(bool)));
}

void MainWidget::initCentralWidget()
{
	m_pGridLayout = new QGridLayout;
	{
		m_pStackWidget = new QStackedWidget(this);
		{
			m_pConnectWidget = new ConnectWidget(this);
			m_pStackWidget->addWidget(m_pConnectWidget);
			m_pStackWidget->setCurrentWidget(m_pConnectWidget);
		}
	}
	m_pGridLayout->addWidget(m_pStackWidget);
	m_pCentralWidget = new QWidget(this);
	m_pCentralWidget->setLayout(m_pGridLayout);
	setCentralWidget(m_pCentralWidget);
}

void MainWidget::onConnectTestTriggered(bool check)
{
	if (!m_pStackWidget || !m_pConnectWidget)
	{
		assert(0);
		return;
	}

	m_pStackWidget->setCurrentWidget(m_pConnectWidget);
}
