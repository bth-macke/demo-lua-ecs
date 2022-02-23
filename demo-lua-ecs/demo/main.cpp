#include <iostream>
#include <lua\lua.hpp>

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	int result = luaL_dostring(L, "print('[Lua] Hello world!')");

	if (result != LUA_OK)
	{
		printf("[C++] Lua error: ");

		if (lua_isstring(L, -1))
		{
			printf("%s", lua_tostring(L, -1));
		}
		else
		{
			printf("Unable do read error.\n");
		}
		lua_pop(L, -1);
	}

	return 0;
}
