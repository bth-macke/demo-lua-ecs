#include "pch.h"
#include "entity.h"

entt::registry& getupvalue_registry(lua_State* L)
{
	entt::registry* pRegistry = nullptr;
	lua_getfield(L, lua_upvalueindex(1), "registry");
	if (lua_isuserdata(L, -1))
	{
		pRegistry = (entt::registry*)lua_touserdata(L, -1);
	}
	lua_pop(L, 1);
	return *pRegistry;
}

int entity_create(lua_State* L)
{
	if (lua_isstring(L, -1))
	{
		entt::registry& registry = getupvalue_registry(L);

		entt::entity entityHandle = registry.create();
		std::string scriptFile = lua_tostring(L, -1);

		lua_State* state = luaL_newstate();

		EntityScript* script = nullptr;
		
		{
			EntityScript** s = reinterpret_cast<EntityScript**>(lua_newuserdata(state))
		}
		

		ScriptComponent& script = registry.emplace<ScriptComponent>(entityHandle, entityHandle, scriptFile, state);

		luaL_openlibs(script.State);
		if (luaL_dofile(script.State, script.ScriptFile.c_str()) != LUA_OK)
		{
			luaC_dumpError(script.State);
		}

		//printf("[C++] entity.create()\t--> { handle=%i, script='%s'}\n", (int)info.Handle, info.Script.c_str());
	}
	else
	{
		std::cout << "[C++] Error 'entity.create': missing script path." << std::endl;
	}
	
	return 0;
}

void luaopen_entity(lua_State* L, entt::registry& registry)
{
	luaL_newmetatable(L, "entity_metatable");

	luaL_Reg functions_meta[] = {
		{ "create", entity_create },
		{ NULL, NULL }
	};

	lua_newtable(L);
	lua_pushlightuserdata(L, &registry);
	lua_setfield(L, -2, "registry");
	luaL_setfuncs(L, functions_meta, 1);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_setglobal(L, "entity");
}

int EntityScript::luaE_getHandle(lua_State* L)
{
	return 0;
}
