#include <iostream>
#include <string>
#include <lua\lua.hpp>
#include "entity.h"

void dumpError(lua_State* L)
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

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_entity(L);

	std::string chunk;
	while (chunk != "exit") {
		std::cout << "> ";
		std::getline(std::cin, chunk);

		int result = luaL_dostring(L, chunk.c_str());

		if (chunk == "clear") {
			system("cls");
		}
		else if (chunk == "exit") {
			std::cout << "Bye bye!" << std::endl;
		}
		else if (result != LUA_OK) {
			dumpError(L);
		}
	}

	return 0;
}
