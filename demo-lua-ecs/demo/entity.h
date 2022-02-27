#pragma once
#include "pch.h"

struct NameComponent
{
	std::string Name;
};

struct ScriptComponent
{
	std::string ScriptFile;
	entt::entity EntityHandle;
	entt::registry* Registry;
};

void luaopen_entity(lua_State* L, entt::registry& registry);
