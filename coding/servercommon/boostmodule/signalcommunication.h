#ifndef __SIGNAL_COMMUNICATION_H__
#define __SIGNAL_COMMUNICATION_H__

#include <boost/signals2/signal.hpp>

#define DEFINE_SIGNAL(signalType, signalName) boost::signals2::signal<signalType> signalName

class SignalSender
{
public:
	enum EnConnectStatus
	{
		CONNECT_OK,
		CONNECT_ERROR,
	};

public:
	template<class ReceiveType>
	int slotConnect(ReceiveType* receiver) { return SignalSender::CONNECT_OK; }
};

#endif // !__SIGNAL_COMMUNICATION_H__
