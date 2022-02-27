#include "pch.h"
#include "entity.h"

int main()
{
	entt::registry registry;
	
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_entity(L, registry);

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
		else if (chunk == "ecsinfo") {
			std::cout << "Entity count: " << registry.size() << std::endl;
		}
		else if (chunk == "ecsupdate") {
			auto view = registry.view<ScriptComponent>();

			std::cout << "[C++] Updating " << view.size() << " entities:" << std::endl;

			view.each([&L](auto script) {

				lua_getglobal(L, "OnUpdate");
				if (lua_isfunction(L, -1))
				{
					lua_pcall(L, 0, 0, 0);
				}
			});
		}
		else if (result != LUA_OK) {
			luaC_dumpError(L);
		}
	}

	return 0;
}
