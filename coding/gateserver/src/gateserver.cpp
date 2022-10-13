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
// Maximum number of connections per server -- Maximum number of connections in available memory
const int g_nConnectMaxCount = 
	int(SystemInfo::getAvailableMemory(SystemInfo::UNIT_B) / sizeof(User) * 1.0);

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
	for (int i = 0; i < CPU_MAX_THREAD - 2; ++i)
	{
		boost::thread tAccServer(BIND(&GateServer::onThreadRunAcceptorIOServer, this));
		tAccServer.detach();
	}
	printf_color(PRINTF_GREEN, 
		"--Gateserver start successed!!!!!!!!!!port[%d],max link count[%d]\n", m_nPort, g_nConnectMaxCount);
	boost::thread tConnect(BIND(&GateServer::runInnnerIOServerOnce, this));
	tConnect.detach();

	boost::thread tUserAync(BIND(&GateServer::runUserIOServerOnce, this));
	tConnect.detach();

	while (1) { THREAD_SLEEP(1); }
}

void GateServer::OnSendToProxySrvByUser(const byte* data, uint size, ullong userSeq)
{
	sendToProxySrv(data, size);
}

void GateServer::accept()
{
	if (!m_pAcceptor)
	{
		LOG_GATESERVER.printLog("m_pAcceptor is NULL");		// I've come here, it seems that I'm not suitable for writing code
		return;
	}

	boost::shared_ptr<User> newUser = boost::make_shared<User>(m_server, m_userIOServer);
	newUser->slotConnect(this);
	if (newUser->getSocket().get() == NULL)
	{
		LOG_GATESERVER.printLog("newUser->getSocket().get() == NULL");			// I've come here, this server like a shi--
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

void GateServer::removeUserRelated(const boost::weak_ptr<User>& user)
{
	if (user.expired())
	{
		LOG_GATESERVER.printLog("user NULL");
		return;
	}
	boost::shared_ptr<User> sUser = user.lock();

	// Store this user seq, which can be assigned to the next connected client
	CommonBoost::UniqueLock lock(m_userSeqMgr.getMutex());
	// delete user from seq map
	MapSeqToUserIter it = m_mapSeqToUser.find(sUser->getSeq());
	if (it != m_mapSeqToUser.end())
	{
		m_mapSeqToUser.erase(it);

		--m_nConnectCount;
		LOG_GATESERVER.printLog("current connect count: [%d], max[%d]", m_nConnectCount.load(), g_nConnectMaxCount);
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
			THREAD_SLEEP(1);
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

void GateServer::runUserIOServerOnce()
{
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_userIOServer));
	while (1)
	{
		THREAD_SLEEP(1);
		try
		{
			m_userIOServer.run();
			break;
		} catch (std::exception& e)
		{
			LOG_GATESERVER.printLog("m_userIOServer run exception!! info[%s] server will re-start!!", e.what());
		}
	}
}

void GateServer::sendServerInfo(const boost::weak_ptr<User>& user)
{
	if(user.expired())
	{
		LOG_GATESERVER.printLog("linking user is NULL");
		return;
	}

	boost::shared_ptr <User> sUser = user.lock();

	DEFINE_BYTE_ARRAY(mode, 1);
	mode[0] = CommonTool::MsgTool::isLittleEndian() ? 0xAE : 0x01;
	sUser->ayncSend(mode, sizeof(mode));
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

void GateServer::sendMsgToClient(const boost::weak_ptr<User>& targetUser, byte* proxyData)
{
	if (targetUser.expired() || !proxyData)
	{
		LOG_GATESERVER.printLog("Pointer NULL");
		return;
	}

	boost::shared_ptr<User> sUser = targetUser.lock();

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
	sUser->ayncSend(proxyData, header->m_nMsgLen);
}

void GateServer::onUserError(
	const boost::weak_ptr<User>& user,
	const CommonBoost::ErrorCode& ec)
{
	if (user.expired())
	{
		LOG_GATESERVER.printLog("input user param expired");
		return;
	}

	boost::shared_ptr<User> sUser = user.lock();

	std::string getIp;
	ushort getPort = 0;
	sUser->getLinkIP(getIp);
	sUser->getLinkPort(getPort);

	// Client shuts down gracefully
	if (ec.value() == GateServer::LOGOUT)
	{
		LOG_GATESERVER.printLog("client[%s : %d] closed",
			getIp.data(),
			getPort);
	}
	else
	{
		if (sUser)
		{
			LOG_GATESERVER.printLog("client[%s : %d] error! ecode[%d],messages[%s]",
				getIp.data(), 
				getPort, 
				ec.value(),
				ec.message().data());
		}
		else
		{
			LOG_GATESERVER.printLog("ecode[%d],messages[%s]", ec.value(),ec.message().data());
		}

	}

	removeUserRelated(user);

}

void GateServer::onThreadRunAcceptorIOServer()
{
	LOG_GATESERVER.printLog("server has run");
	CommonBoost::WorkPtr work(new CommonBoost::IOServer::work(m_server));
 
	/*
	If an exception is caught, an error may occur. The reason for this error is that the connection is disconnected immediately after the client establishes the connection, 
	before the server calls remote_endpoint, resulting in a return failure.

	Error info£º
		terminate called after throwing an instance of 'boost::wrapexcept<boost::system::system_error>'
		what():  remote_endpoint: Transport endpoint is not connected
		Aborted
	*/
	while(1)
	{
		THREAD_SLEEP(1);
		try
		{
			m_server.run();
			break;
		}catch (std::exception& e)
		{
			LOG_GATESERVER.printLog("m_server run exception!! info[%s] server will re-start!!",e.what());
			printf_color(PRINTF_RED, "%s: m_server run exception!! info[%s] server will re-start!!\n", __FUNCTION__, e.what());
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

	// send a byte info,tell proxy server my identity
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

		// Do other exceptions
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
		THREAD_SLEEP(1);
		memset(m_bytesInnerSrvOnceMsg, 0, sizeof(m_bytesInnerSrvOnceMsg));

		m_msgHeader = *(MsgHeader*)(m_bytesInnerSrvBuffer + m_nHasReadProxyDataSize);

		// Judgment of the maximum length of a protocol
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

		// if this msg is heart with proxy server
		if (m_msgHeader.m_nMsgType >= MSG_TYPE_GATE_PROXY_HEART_GP && 
			m_msgHeader.m_nMsgType < MSG_IN_TYPE_MAX &&
			m_msgHeader.m_nMsgType == MSG_TYPE_GATE_PROXY_HEART_PG
			)
		{
			SingleToProxyMsgHandler::callHandler(
				m_msgHeader.m_nMsgType, 
				(const byte*)this, 
				m_bytesInnerSrvOnceMsg + sizeof(MsgHeader),
				m_msgHeader.m_nMsgLen - sizeof(MsgHeader));
			GATE_SERVER_READ_MSG_CONTINUE;
		}

		// Reassemble the message and send it to the client
		MapSeqToUserIter userIt = m_mapSeqToUser.find(m_msgHeader.m_nClientSrcSeq);
		if (userIt != m_mapSeqToUser.cend())
		{
			boost::shared_ptr<User> callbackUser = userIt->second;
			if (callbackUser)
			{
				sendMsgToClient(callbackUser, m_bytesInnerSrvOnceMsg);
			}
		}
		
		

		m_nHasReadProxyDataSize += m_msgHeader.m_nMsgLen;

	}

	readFromProxySrv();
}

void GateServer::onAcceptHandler(
	const CommonBoost::ErrorCode& err,
	const boost::weak_ptr<User>& user
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

	if (user.expired())
	{
		LOG_GATESERVER.printLog("linking user is NULL");
		accept();
		return;
	}
	boost::shared_ptr<User> sUser = user.lock();

	++m_nConnectCount;

	std::string ip;
	ushort port = 0;
	sUser->getLinkIP(ip);
	sUser->getLinkPort(port);

	sUser->setSeq(m_userSeqMgr.genSeq(ip, port));
	m_mapSeqToUser[sUser->getSeq()] = sUser;

	
	if (!ip.empty() && port != 0)
	{
		LOG_GATESERVER.printLog("new client[%s : %d](seq=%lld) connect succ, client has link count[%d]",
			ip.data(),
			port,
			sUser->getSeq(),
			m_nConnectCount.load());
	}
	sendServerInfo(user);
	sUser->ayncRead();
	sUser->checkUserValid();
	accept();
}


