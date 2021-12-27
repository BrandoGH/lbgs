#ifndef __SIGNAL_COMMUNICATION_H__
#define __SIGNAL_COMMUNICATION_H__

#include <boost/signals2/signal.hpp>

#define DEFINE_SIGNAL(signalType, signalName) boost::signals2::signal<signalType> signalName

#endif // !__SIGNAL_COMMUNICATION_H__
