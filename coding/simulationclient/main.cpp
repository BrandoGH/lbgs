#include "src/mainwidget.h"
#include "servercommon/commontool/msgtool/msgtool.h"
#include "servercommon/basedef.h"

#include <iostream>
#include <QtWidgets/QApplication>


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	// test
	short s = 20;
	char* cc = (char*)&s;
	DEFINE_BYTE_ARRAY(c,2);
	CommonTool::MsgTool::Little2Big(s, c);
	short ss = *(short*)(void*)c;

	MainWidget mw;
	mw.show();

	return app.exec();
}