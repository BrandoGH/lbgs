#include "src/mainwidget.h"
#include "servercommon/commontool/msgtool/msgtool.h"
#include "servercommon/basedef.h"

#include <iostream>
#include <QtWidgets/QApplication>


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	MainWidget mw;
	mw.show();

	return app.exec();
}