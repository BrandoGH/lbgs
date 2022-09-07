#include <gtest/gtest.h>
#include <iostream>
#include <servercommon/boostmodule/basedef.h>
#include <servercommon/timermodule/timer.h>

extern "C"
{
#include <lua.h> 
#include <lualib.h> 
#include <lauxlib.h> 
}

// 定时器用法
namespace
{
class TimerTest : public Timer
{
public:
	virtual void timeoutRun()
	{
		std::cout << 454545 << std::endl;
	}

};
TEST(Timer_Example, test)
{
	/*TimerTest pc;
	pc.setInterval(500);
	pc.start();
	getchar();*/
}

// lua例子
TEST(lua_test,lua)
{
	printf("============================================lua_test============================================\n");


	lua_State* L = luaL_newstate();
	luaL_dostring(L, "x = 1200");
	lua_getglobal(L, "x");
	lua_Number x = lua_tonumber(L, 1);
	printf("lua sya x = %d\n", int(x));
	lua_close(L);
}

TEST(main_test, test)
{

}

}
