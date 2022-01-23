#include "export.h"

GATESERVER_EXPORT GateServerConfig* getGateServerConfig()
{
	return new GateServerConfig;
}