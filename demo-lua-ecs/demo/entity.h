#pragma once
#include "pch.h"

struct EntityScript
{
	lua_State* State;
	std::string ScriptFile;
	entt::entity EntityHandle;
	entt::registry& Registry;

	int luaE_getHandle(lua_State* L);
};

struct ScriptComponent
{
	EntityScript* Script;
};



void luaopen_entity(lua_State* L, entt::registry& registry);

