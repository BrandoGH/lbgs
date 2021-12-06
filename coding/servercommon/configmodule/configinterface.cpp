#include "configinterface.h"


ConfigInterface::ConfigInterface()
{
}

ConfigInterface::~ConfigInterface()
{
}

bool ConfigInterface::load(const std::string& path)
{
	READ_XML(CONFIG_BASE_DIR + path, m_ptree);
	if (m_ptree.empty())
	{
		LOG_SERVER_COMMON_CONFIG.printLog("m_ptree is empty");
		return false;
	}
	return true;
}

void ConfigInterface::getPTreeNode(const std::string& nodeName, CommonBoost::PTree* outPTree)
{
	*outPTree = m_ptree.get_child(nodeName);

	if (outPTree == NULL)
	{
		LOG_SERVER_COMMON_CONFIG.printLog("outPTree is NULL");
		BOOST_ASSERT(outPTree != NULL);
	}

}
