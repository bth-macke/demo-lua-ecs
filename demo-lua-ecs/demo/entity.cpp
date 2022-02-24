#include "pch.h"
#include "entity.h"

void luaopen_me(lua_State* L, ScriptComponent& script, entt::registry& registry)
{
	lua_newtable(L);

	lua_pushinteger(L, (int)script.EntityHandle);
	lua_setfield(L, -2, "handle");

	luaL_Reg methods[] =
	{
		{ NULL, NULL }
	}
}

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

		{
			luaL_openlibs(state);
			luaopen_entity(state, registry);

			lua_pushinteger(state, (int)entityHandle);
			lua_setglobal(state, "me");

			if (luaL_dofile(state, scriptFile.c_str()) != LUA_OK)
			{
				luaC_dumpError(state);
			}
		}
		

		ScriptComponent& script = registry.emplace<ScriptComponent>(entityHandle, state, scriptFile, entityHandle, &registry);

		//printf("[C++] entity.create()\t--> { handle=%i, script='%s'}\n", (int)info.Handle, info.Script.c_str());

		lua_pushinteger(L, (int)entityHandle);
		return 1;
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
