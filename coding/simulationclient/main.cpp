#include "src/mainwindowobj.h"
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/basedef.h>
#include <servercommon/sysinfomodule/minidump/minidump.h>

#include <iostream>
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	assert(LbgsMinisDump::autoDump() != NULL);
	QApplication app(argc, argv);

	MainWindowObj mw;
	mw.show();

	return app.exec();
}