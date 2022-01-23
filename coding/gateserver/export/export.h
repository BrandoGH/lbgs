#ifndef __GATESERVER_EXPORT_H__
#define __GATESERVER_EXPORT_H__

#include "gateserver/config/gateserverconfig.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	GATESERVER_EXPORT GateServerConfig* getGateServerConfig();
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ! __GATESERVER_EXPORT_H__
