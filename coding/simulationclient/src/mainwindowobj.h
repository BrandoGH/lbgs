#ifndef __MAIN_WINDOW_OBJ_H_
#define __MAIN_WINDOW_OBJ_H_

#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtUiTools/QUiLoader>

class QMainWindow;
class MainWindowObj : public QObject
{
	Q_OBJECT

public:
	MainWindowObj(QObject* parent = NULL);
	~MainWindowObj();

	void show();

private:
	void loadMainWindow();

private:
	QUiLoader m_uiLoader;
	QFile m_uiFile;

	QMainWindow* m_mw;
};

#endif // !__MAIN_WINDOW_OBJ_H_
