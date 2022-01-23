#ifndef __GATESERVER_GLOBAL_H__
#define __GATESERVER_GLOBAL_H__

#ifndef GATESERVER_EXPORT
#if defined GATESERVER_LIB 
#define GATESERVER_EXPORT __declspec(dllexport)
#else
#define GATESERVER_EXPORT __declspec(dllimport)
#endif
#endif // !GATESERVER_EXPORT

#endif // !__GATESERVER_GLOBAL_H__