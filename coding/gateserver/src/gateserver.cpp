#include "gateserver.h"

#include <configmodule/gateserverconfig/gateserverconfig.h>
#include <configmodule/configmanager.h>
#include <servercommon/logmodule/logdef.h>
#include <servercommon/sysinfomodule/sysinfo.h>
#include <servercommon/commontool/msgtool/msgtool.h>
#include <servercommon/proxyserverconfig.h>
#include <servercommon/basedef.h>
#include <exception>
#include <msgmodule/singletoproxymsghandler.h>
#include <logicserver/communicationmsg/msgheart.h>

#define GATE_SERVER_READ_MSG_CONTINUE \
	m_nHasReadProxyDataSize += m_msgHeader.m_nMsgLen; \
	continue

namespace 
{
// 单台服务器最大连接数可用内存的最大连接数
const int g_nConnectMaxCount = 
	int(SystemInfo::getAvailableMemory(SystemInfo::UNIT_B) / MsgBuffer::g_nReadBufferSize * 1.0); 

}

using CommonBoost::Endpoint;
using CommonBoost::TCP;
GateServer::GateServer()
{
	initData();
	if(!CONFIG_MGR->GetGateServerConfig())
	{
		assert(0);
	}
	const GateServerConfigInfo* pCfgInfo = CONFIG_MGR->GetGateServerConfig()->getConfigInfo();
	if(pCfgInfo)
	{
		m_nPort = pCfgInfo->port;
		m_pAcceptor = new CommonBoost::Acceptor(
			m_server,
			CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
		accept();
		LOG_GATESERVER.printLog("has run gateserver succ");
	}

}

GateServer::GateServer(int port)
{
	initData();
	m_nPort = port;
	m_pAcceptor = new CommonBoost::Acceptor(
		m_server,
		CommonBoost::Endpoint(CommonBoost::TCP::v4(), m_nPort));
	accept();
	LOG_GATESERVER.printLog("has run gateserver succ");
}

GateServer::~GateServer()
{
	if (m_pAcceptor)
	{
		delete m_pAcceptor;
		m_pAcceptor = NULL;
	}
	LOG_GATESERVER.printLog("run ~GateServer()");
}

void GateServer::start()
{
	LOG_GATESERVER.printLog("gateserver has start,run sub thread count[%d]", CPU_MAX_THREAD);
	LOG_GATESERVER.printLog("gateserver has start,port[%d],max link count[%d]",
		m_nPort, 
		g_nConnectMaxCount
	);
	for (int i = 0; i < CPU_MAX_THREAD - 1; ++i)
	{
		boost::thread tAccServer(BIND(&GateServer::onThreadRunAcceptorIOServer, this));
		tAccServer.detach();
	}
	printf_color(PRINTF_GREEN, 
		"--Gateserver start successed!!!!!!!!!!port[%d],max link count[%d]\n", m_nPort, g_nConnectMaxCount);
	boost::thread tConnect(BIND(&GateServer::runInnnerIOServerOnce, this));
	tConnect.detach();
	while (1);
}

void GateServer::OnSendToProxySrvByUser(const byte* data, uint size, int userSeq)
{
	sendToProxySrv(data, size);
}

void GateServer::accept()
{
	if (!m_pAcceptor)
	{
		LOG_GATESERVER.printLog("m_pAcceptor is NULL");		// 这里都能跑到，还写什么代码，不如去搬砖
		return;
	}

	boost::shared_ptr<User> newUser = boost::make_shared<User>(m_server);
	newUser->slotConnect(this);
	if (newUser->getSocket().get() == NULL)
	{
		LOG_GATESERVER.printLog("newUser->getSocket().get() == NULL");			// 都跑到这里了，服务器是不是有问题
		return;
	}
	m_pAcceptor->async_accept(*(newUser->getSocket()), BIND(&GateServer::onAcceptHandler, this, boost::placeholders::_1, newUser));
}

void GateServer::initData()
{
	const ProxyServerConfigInfo info = *(CONFIG_MGR->GetProxyServerConfig()->getConfigInfo());
	m_pAcceptor = NULL;
	m_nConnectCount = 0;
	m_nPort = 0;
	m_bInnerRunOnce = false;
	m_bConnectProxySrv = false;
	m_nHasReadProxyDataSize = 0;
	memset(m_bytesInnerSrvBuffer, 0, MsgBuffer::g_nReadBufferSize);
	memset(m_bytesInnerSrvOnceMsg, 0, MsgBuffer::g_nOnceMsgSize);
	initInnerClient();
	m_innerSrvHeart.setGateServer(this);
	m_innerSrvHeart.setInterval(info.heart_time);
}

void GateServer::removeUserRelated(boost::shared_ptr<User> user)
{
	if (!user.get())
	{
		LOG_GATESERVER.printLog("user NULL");
		return;
	}
	// 存储这个用户seq，可以分配给下一个连接的客户端
	CommonBoost::UniqueLock lock(m_userSeqMgr.getMutex());
	m_userSeqMgr.pushAsideSeq(user->getSeq());

	// 从seq-用户映射表删除
	MapSeqToUserIter it = m_mapSeqToUser.find(user->getSeq());
	if (it != m_mapSeqToUser.end())
	{
		m_mapSeqToUser.erase(it);
	}
}

void GateServer::initInnerClient()
{
	if(!CONFIG_MGR->GetProxyServerConfig())
	{
		return;
	}
	const ProxyServerConfigInfo info = *(CONFIG_MGR->GetProxyServerConfig()->getConfigInfo());
	
	m_pInnerSocket = boost::make_shared<CommonBoost::Socket>(m_innerServer);
	m_pInnerStrand = boost::make_shared<CommonBoost::Strand>(m_innerServer);
	m_innerEndpoint = CommonBoost::Endpoint(
		boost::asio::ip::address::from_string(info.ip),info.port
		);
	connectInnerServer();
}

void GateServer::connectInnerServer()
{
	if(!m_pInnerSocket)
	{
		LOG_GATESERVER.printLog("m_pInnerSocket is NULL");
		return;
	}

	if(m_bConnectProxySrv)
	{
		closeInnerSocket();
	}
	m_bConnectProxySrv = false;
	m_pInnerSocket->async_connect(m_innerEndpoint, BIND(&GateServer::onConnectInnerServer, this, boost::placeholders::_1));
}

void GateServer::closeInnerSocket()
{
	if(!m_pInnerSocket)
	{
		LOG_GATESERVER.printLog("m_pInnerSocket is NULL");
		return;
	}
	m_pInnerSocket->close();
}

void GateServer::runInnnerIOServerOnce()
{
	if(!m_bInnerRunOnce)
	{
		CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_innerServer));
		m_bInnerRunOnce = true;
		while(1)
		{
			try
			{
				m_innerServer.run();
				break;
			}
			catch(std::exception& e)
			{
				LOG_GATESERVER.printLog("m_innerServer run exception!! info[%s] server will re-start!!", e.what());
			}
		}
	}
}

void GateServer::sendServerInfo(const boost::shared_ptr<User>& user)
{
	if(!user)
	{
		LOG_GATESERVER.printLog("linking user is NULL");
		return;
	}
	DEFINE_BYTE_ARRAY(mode, 1);
	mode[0] = CommonTool::MsgTool::isLittleEndian() ? 0xAE : 0x01;
	user->ayncSend(mode, sizeof(mode));
}

void GateServer::sendToProxySrv(const byte* data, uint size)
{
	if(!m_pInnerSocket || !m_pInnerStrand)
	{
		LOG_GATESERVER.printLog("m_pInnerSocket NULL || m_pInnerStrand NULL");
		return;
	}

	m_pInnerSocket->async_write_some(
		MSG_BUFFER(data, size),
		m_pInnerStrand->wrap(BIND(&GateServer::onProxySrvSend, this, boost::placeholders::_1, boost::placeholders::_2))
	);
}

void GateServer::readFromProxySrv()
{
	if (!m_pInnerSocket || !m_pInnerStrand)
	{
		LOG_GATESERVER.printLog("m_pInnerSocket NULL || m_pInnerStrand NULL");
		return;
	}

	memset(m_bytesInnerSrvBuffer, 0, sizeof(m_bytesInnerSrvBuffer));
	m_pInnerSocket->async_read_some(
		MSG_BUFFER(m_bytesInnerSrvBuffer, sizeof(m_bytesInnerSrvBuffer)),
		m_pInnerStrand->wrap(BIND(&GateServer::onProxySrvRead, this, boost::placeholders::_1, boost::placeholders::_2))
	);
}

void GateServer::sendMsgToClient(const boost::shared_ptr<User> targetUser, byte* proxyData)
{
	if (!targetUser.get() || !proxyData)
	{
		LOG_GATESERVER.printLog("Pointer NULL");
		return;
	}

	MsgHeader* header = (MsgHeader*)proxyData;
	if (!header)
	{
		LOG_GATESERVER.printLog("header NULL");
		return;
	}
	header->m_nMsgLen += sizeof(MsgEnder);

	MsgEnder ender;
	int nHeartBodySize = header->m_nMsgLen - sizeof(MsgEnder);
	if (!CommonTool::MsgTool::data2Md5(proxyData, nHeartBodySize, ender.m_bytesMD5))
	{
		LOG_GATESERVER.printLog("send msg to client---MD5 error!!");
		return;
	}
	memmove(proxyData + nHeartBodySize, (const char*)&ender, sizeof(MsgEnder));
	targetUser->ayncSend(proxyData, header->m_nMsgLen);
}

void GateServer::onUserError(
	boost::shared_ptr<User> user,
	const CommonBoost::ErrorCode& ec)
{
	--m_nConnectCount;
	LOG_GATESERVER.printLog("current connect count: [%d], max[%d]", m_nConnectCount.load(), g_nConnectMaxCount);

	std::string getIp;
	ushort getPort = 0;

	bool bUserValid = false;
	if (user.get() != NULL)
	{
		bUserValid = true;
		user->getLinkIP(getIp);
		user->getLinkPort(getPort);
	}

	removeUserRelated(user);

	// 客户端正常关闭
	if (ec.value() == GateServer::LOGOUT)
	{
		LOG_GATESERVER.printLog("client[%s : %d] closed",
			getIp.data(), 
			getPort);
 		user->closeSocket();
		return;
	}
	else
	{
		// 其他错误
		if (bUserValid)
		{
			LOG_GATESERVER.printLog("client[%s : %d] error! ecode[%d],messages[%s]",
				getIp.data(), 
				getPort, 
				ec.value(),
				ec.message().data());
			user->closeSocket();
		}
		else
		{
			LOG_GATESERVER.printLog("ecode[%d],messages[%s]", ec.value(),ec.message().data());
		}

	}

}

void GateServer::onThreadRunAcceptorIOServer()
{
	LOG_GATESERVER.printLog("server has run");
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_server));
 
	/*
	捕获异常，可能会出现一个错误，这个错误的原因在于客户端建立连接以后一瞬间，服务端调用remote_endpoint前，就断开了链接，导致返回失败
	错误信息如下：
		terminate called after throwing an instance of 'boost::wrapexcept<boost::system::system_error>'
		what():  remote_endpoint: Transport endpoint is not connected
		Aborted
	*/
	while(1)
	{
		try
		{
			m_server.run();
			break;
		}catch (std::exception& e)
		{
			LOG_GATESERVER.printLog("m_server run exception!! info[%s] server will re-start!!",e.what());
		}
	}
	
}

void GateServer::onConnectInnerServer(const CommonBoost::ErrorCode& err)
{
	if(err)
	{
		LOG_GATESERVER.printLog("Please run ProxyServer first.......");
		printf_color(PRINTF_RED,"\nPlease run ProxyServer first.......\n");
		return;
	}

	if(m_pInnerSocket.get() == NULL)
	{
		LOG_GATESERVER.printLog("m_pInnerSocket is NULL");
		return;
	}

	LOG_GATESERVER.printLog("link proxy server succ");
	printf_color(PRINTF_GREEN, "\nlink proxy server succ\n");
	m_bConnectProxySrv = true;

	m_innerSrvHeart.start();

	// 发送一个字节，告诉代理服自己的身份
	DEFINE_BYTE_ARRAY(firstData, 1);
	firstData[0] = MsgHeader::F_GATESERVER;
	sendToProxySrv(firstData, 1);

	readFromProxySrv();
}

void GateServer::onProxySrvSend(const CommonBoost::ErrorCode& ec, uint readSize)
{
	if(ec)
	{
		LOG_GATESERVER.printLog("error value[%d],send size[%d], message[%s]",
			ec.value(),
			readSize,
			ec.message().data());

		// 异常处理
	}
}

void GateServer::onProxySrvRead(const CommonBoost::ErrorCode& ec, uint readSize)
{
	if (ec)
	{
		LOG_GATESERVER.printLog("ecode[%d],messages[%s]",
			ec.value(),
			ec.message().data());
		connectInnerServer();
		return;
	}
	if(readSize <= 0 || readSize > MsgBuffer::g_nReadBufferSize)
	{
		LOG_GATESERVER.printLog("size error,readSize[%d],g_nReadBufferSize[%d]", readSize, MsgBuffer::g_nReadBufferSize);
		return;
	}

	MsgHeader m_msgHeader;
	m_nHasReadProxyDataSize = 0;

	while (m_nHasReadProxyDataSize < readSize)
	{
		memset(m_bytesInnerSrvOnceMsg, 0, sizeof(m_bytesInnerSrvOnceMsg));

		m_msgHeader = *(MsgHeader*)(m_bytesInnerSrvBuffer + m_nHasReadProxyDataSize);

		// 一条协议最大长度判断
		if (m_msgHeader.m_nMsgLen > MsgBuffer::g_nOnceMsgSize ||
			m_msgHeader.m_nMsgLen <= 0)
		{
			LOG_GATESERVER.printLog("msgtype[%d] size[%d] error, read buff[%s]",
				m_msgHeader.m_nMsgType,
				m_msgHeader.m_nMsgLen,
				m_bytesInnerSrvBuffer);
			m_nHasReadProxyDataSize++;
			continue;
		}
		memmove(m_bytesInnerSrvOnceMsg, m_bytesInnerSrvBuffer + m_nHasReadProxyDataSize, m_msgHeader.m_nMsgLen);

		if (m_msgHeader.m_nProxyer != MsgHeader::F_PROXYSERVER ||
			m_msgHeader.m_nReceiver != MsgHeader::F_GATESERVER
			)
		{
			LOG_GATESERVER.printLog("MsgHeader Info Error");
			GATE_SERVER_READ_MSG_CONTINUE;
		}

		// 如果此条协议是只和网关通信的
		if (m_msgHeader.m_nMsgType >= MSG_TYPE_GATE_PROXY_HEART_GP && 
			m_msgHeader.m_nMsgType < MSG_IN_TYPE_MAX
			)
		{
			SingleToProxyMsgHandler::callHandler(
				m_msgHeader.m_nMsgType, 
				(const byte*)this, 
				m_bytesInnerSrvOnceMsg + sizeof(MsgHeader),
				m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
			GATE_SERVER_READ_MSG_CONTINUE;
		}

		boost::shared_ptr<User> callbackUser = m_mapSeqToUser[m_msgHeader.m_nClientSrcSeq];
		if (callbackUser)
		{
			sendMsgToClient(callbackUser, m_bytesInnerSrvBuffer);
		}
		

		m_nHasReadProxyDataSize += m_msgHeader.m_nMsgLen;

	}

	readFromProxySrv();
}

void GateServer::onAcceptHandler(
	const CommonBoost::ErrorCode& err,
	const boost::shared_ptr<User>& user
	)
{
	if (m_nConnectCount > g_nConnectMaxCount)
	{
		LOG_GATESERVER.printLog("connect size has over [%d]", g_nConnectMaxCount);
		accept();
		return;
	}

	if (err)
	{
		LOG_GATESERVER.printLog("new client connect error value[%d],message[%s]", err.value(), err.message().data());
		accept();
		return;
	}
	if (!user)
	{
		LOG_GATESERVER.printLog("linking user is NULL");
		accept();
		return;
	}

	++m_nConnectCount;

	CommonBoost::UniqueLock lock(m_userSeqMgr.getMutex());
	user->setSeq(m_userSeqMgr.getAvailableSeq());
	lock.unlock();
	m_mapSeqToUser[user->getSeq()] = user;

	std::string ip;
	ushort port = 0;
	user->getLinkIP(ip);
	user->getLinkPort(port);
	if (!ip.empty() && port != 0)
	{
		LOG_GATESERVER.printLog("new client[%s : %d] connect succ, client has link count[%d]",
			ip.data(),
			port,
			m_nConnectCount.load());
	}
	sendServerInfo(user);
	user->ayncRead();
	accept();
}


