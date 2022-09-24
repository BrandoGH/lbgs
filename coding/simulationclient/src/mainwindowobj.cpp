#include "mainwindowobj.h"

#include <QtWidgets/QMainWindow>

MainWindowObj::MainWindowObj(QObject* parent)
	: QObject(parent)
{
	loadMainWindow();
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

void MainWindowObj::loadMainWindow()
{
	m_uiFile.setFileName(UI_FILE_DIR"mainwindow.ui");
	m_uiFile.open(QFile::ReadOnly);
	m_mw = (QMainWindow*)m_uiLoader.load(&m_uiFile);
	m_uiFile.close();
}
