#ifndef __CONFIG_INTERFACE_H__
#define __CONFIG_INTERFACE_H__

#include "boostcommondef/basedef.h"

#include <logmodule/logdef.h>

#define LOAD_NODE(nodeName,fun)																						\
do																													\
{																													\
	if (load(path))																									\
	{																												\
		CommonBoost::PTree node;																					\
		getPTreeNode(nodeName, &node);																				\
		int ret = fun(node);																						\
		if (ret != 0)																								\
		{																											\
			LOG_SERVER_COMMON_CONFIG.printLog("config init fail! function[%s],return error value[%d]", #fun, ret);	\
			return ret;																								\
		}																											\
	}																												\
} while (0)

class ConfigInterface
{
public:
	enum ErrorCode
	{
		INIT_OK = 0,
	};

public:
	ConfigInterface();
	virtual ~ConfigInterface();

	virtual int init(const std::string& path) = 0;
	bool load(const std::string& path);
	void getPTreeNode(const std::string& nodeName, CommonBoost::PTree* outPTree);

private:
	CommonBoost::PTree m_ptree;

};

#endif // !__CONFIG_INTERFACE_H__
