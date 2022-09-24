#include "src/mainwindow.h"
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/basedef.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>

#include <iostream>
#include <QtWidgets/QApplication>

#include <QtUiTools/QUiLoader>
#include <QtCore/QFile>

int main(int argc, char* argv[])
{
	assert(LbgsMinisDump::autoDump() != NULL);
	QApplication app(argc, argv);

	/*MainWidget mw;
	mw.show();*/

	/*QUiLoader loader;
	QFile file("F:\\code\\lbgs\\coding\\simulationclient\\test.ui");
	file.open(QFile::ReadOnly);
	QWidget* widget = loader.load(&file, NULL);
	if (widget)
	{
		widget->show();
	}
	file.close();*/

	return app.exec();
}