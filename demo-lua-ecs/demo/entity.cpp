#include "pch.h"
#include "entity.h"

void luaopen_me(lua_State* L, ScriptComponent& script, entt::registry& registry)
{
	lua_newtable(L);

	lua_pushinteger(L, (int)script.EntityHandle);
	lua_setfield(L, -2, "handle");

	//luaL_Reg methods[] =
	//{
	//	{ NULL, NULL }
	//}
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

lua_State* getupvalue_state(lua_State* L)
{
	lua_State* state = nullptr;
	lua_getfield(L, lua_upvalueindex(1), "state");
	if (lua_isuserdata(L, -1))
	{
		state = (lua_State*)lua_touserdata(L, -1);
	}
	lua_pop(L, 1);
	return state;
}

int entity_create(lua_State* L)
{
	if (lua_isstring(L, -1))
	{
		entt::registry& registry = getupvalue_registry(L);
		lua_State* globalState = getupvalue_state(L);
		entt::entity entityHandle = registry.create();
		std::string scriptFile = lua_tostring(L, -1);

		if (luaL_dofile(L, scriptFile.c_str()) != LUA_OK)
		{
			luaC_dumpError(L);
		}

		ScriptComponent& script = registry.emplace<ScriptComponent>(entityHandle, scriptFile, entityHandle, &registry);
		//lua_pushinteger(L, (int)entityHandle);

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

	lua_pushlightuserdata(L, L);
	lua_setfield(L, -2, "state");

	luaL_setfuncs(L, functions_meta, 1);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_newtable(L);
	lua_setfield(L, -2, "entities");

	lua_setglobal(L, "entity");
}
