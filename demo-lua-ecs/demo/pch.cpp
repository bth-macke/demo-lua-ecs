#include "pch.h"

void luaC_dumpError(lua_State* L)
{
	std::cout << "Lua error: ";
	if (lua_gettop(L) > 0 && lua_isstring(L, -1))
	{
		std::cout << lua_tostring(L, -1) << std::endl;
	}
	else
	{
		std::cout << "unable to read error." << std::endl;
	}
	lua_pop(L, -1);
}
