#ifndef __LOGIC_SERVER_H
#define __LOGIC_SERVER_H

class LogicServer
{
public:
	LogicServer();
	LogicServer(int port);
	~LogicServer();

	void start();

private:
	void initData();

	// ���ӵ����������
	void initInnerClient();
};

#endif  //__LOGIC_SERVER_H
