#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/basedef.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <iosfwd>
#include <commontool/serializememory/serializememory.hpp>

struct BoostTestParam
{
	char name[15] = "test name";
	char passwd[15] = "1245457ff";
	int age = 15;
};
typedef byte NeedDefineBoostTestParamHex[sizeof(BoostTestParam)];
namespace boost{namespace serialization
{
template<class Archive>
void serialize(Archive& ar, BoostTestParam& p, const unsigned int version)
{
	ar& p.name;
	ar& p.passwd;
	ar& p.age;
}

}}

namespace
{
TEST(Boost_Serialization_Example, test)
{
	/*BoostTestParam p1;
	std::ostringstream os;
	boost::archive::binary_oarchive oa(os);
	oa << p1;

	std::string strSeria = os.str();

	BoostTestParam p2;
	std::istringstream is(strSeria);
	boost::archive::binary_iarchive ia(is);
	ia >> p2;
	return;*/
}

TEST(Struct_Serialization_Example, test)
{
	// hex string <----> binnary data
	//BoostTestParam p;
	//std::string strHex = SerialzeMem::dataToHexStr((const byte*)&p, sizeof(BoostTestParam));

	//byte BoostTestParamHex[sizeof(BoostTestParam)];
	//memset(BoostTestParamHex, 0, sizeof(BoostTestParamHex));
	//uint retSize = 0;
	//SerialzeMem::hexStrToData(strHex, BoostTestParamHex, retSize);
	//BoostTestParam p2 = *(BoostTestParam*)(BoostTestParamHex);

	//// serialize
	//std::string seri = SerialzeMem::serializationToHexString<BoostTestParam>(&p);
	//BoostTestParam p3;
	//bool b = SerialzeMem::unSerializationFromHexString(seri, p3);
	//

	//NeedDefineBoostTestParamHex hexStr;
	//SerialzeMem::serializationToMemData<BoostTestParam>(&p, hexStr, sizeof(hexStr));

	//BoostTestParam pp;
	//SerialzeMem::unSerializationFromMemData(pp, hexStr);

}

}

