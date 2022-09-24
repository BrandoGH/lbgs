#ifndef __MAIN_WIDGET_H__
#define __MAIN_WIDGET_H__

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>

class QMenuBar;
class QMenu;
class QAction;
class QStackedWidget;
class QGridLayout;
class QWidget;
class ConnectWidget;

class MainWidget : public QMainWindow
{
	Q_OBJECT

public:
	MainWidget(QMainWindow* parent = NULL);
	~MainWidget();

private:
	void initMenuBar();
	void initCentralWidget();

private slots:
	void onConnectTestTriggered(bool check);

private:
	QMenuBar* m_pMenuBar;
	QMenu* m_pMenu;
	QAction* m_pAcConnectTest;

	QWidget* m_pCentralWidget;
	QGridLayout* m_pGridLayout;
	QStackedWidget* m_pStackWidget;

	ConnectWidget* m_pConnectWidget;
};

#endif // !__MAIN_WIDGET_H__
