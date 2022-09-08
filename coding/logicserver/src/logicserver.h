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

	// 连接到代理服操作
	void initInnerClient();
};

#endif  //__LOGIC_SERVER_H
