#ifndef __PROXYSERVER_GLOBAL_H__
#define __PROXYSERVER_GLOBAL_H__

#ifndef PROXYSERVER_EXPORT
#if defined PROXYSERVER_LIB 
#define PROXYSERVER_EXPORT __declspec(dllexport)
#else
#define PROXYSERVER_EXPORT __declspec(dllimport)
#endif
#endif // !PROXYSERVER_EXPORT

#endif // !__PROXYSERVER_GLOBAL_H__