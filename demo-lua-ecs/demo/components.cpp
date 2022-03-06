#include "components.h"

void luaC_pushname(lua_State* L, const NameComponent& component)
{
	lua_pushstring(L, component.Name.c_str());
}

NameComponent luaC_toname(lua_State* L, int i)
{
	NameComponent component;
	component.Name = lua_tostring(L, -1);
	lua_pop(L, 1);

	return component;
}

void luaC_pushtransform(lua_State* L, const TransformComponent& component)
{
	// Transform
	lua_newtable(L);

	// Position
	lua_newtable(L);
	lua_pushnumber(L, component.Position.x); lua_seti(L, -2, 1);
	lua_pushnumber(L, component.Position.y); lua_seti(L, -2, 2);
	lua_pushnumber(L, component.Position.z); lua_seti(L, -2, 3);
	lua_setfield(L, -2, "position");

	// Position
	lua_newtable(L);
	lua_pushnumber(L, component.Rotation.x); lua_seti(L, -2, 1);
	lua_pushnumber(L, component.Rotation.y); lua_seti(L, -2, 2);
	lua_pushnumber(L, component.Rotation.z); lua_seti(L, -2, 3);
	lua_setfield(L, -2, "rotation");

	// Position
	lua_newtable(L);
	lua_pushnumber(L, component.Scale.x); lua_seti(L, -2, 1);
	lua_pushnumber(L, component.Scale.y); lua_seti(L, -2, 2);
	lua_pushnumber(L, component.Scale.z); lua_seti(L, -2, 3);
	lua_setfield(L, -2, "scale");
}

TransformComponent luaC_totransform(lua_State* L, int i)
{
	TransformComponent component;

	lua_getfield(L, -1, "position");
	lua_geti(L, -1, 1); component.Position.x = lua_tonumber(L, -1);
	lua_geti(L, -1, 2); component.Position.y = lua_tonumber(L, -1);
	lua_geti(L, -1, 3); component.Position.z = lua_tonumber(L, -1);
	lua_pop(L, 1); // Pop position

	lua_getfield(L, -1, "rotation");
	lua_geti(L, -1, 1); component.Rotation.x = lua_tonumber(L, -1);
	lua_geti(L, -1, 2); component.Rotation.y = lua_tonumber(L, -1);
	lua_geti(L, -1, 3); component.Rotation.z = lua_tonumber(L, -1);
	lua_pop(L, 1); // Pop rotation

	lua_getfield(L, -1, "scale");
	lua_geti(L, -1, 1); component.Scale.x = lua_tonumber(L, -1);
	lua_geti(L, -1, 2); component.Scale.y = lua_tonumber(L, -1);
	lua_geti(L, -1, 3); component.Scale.z = lua_tonumber(L, -1);
	lua_pop(L, 1); // Pop scale
	lua_pop(L, 1); // Pop transform

	return component;
}

void luaC_pushscript(lua_State* L, const ScriptComponent& component)
{
	// ???
}

ScriptComponent luaC_toscript(lua_State* L, int i)
{
	ScriptComponent component;

	component.ScriptPath = lua_tostring(L, -1);
	lua_pop(L, 1); // Pop file path

	return ScriptComponent();
}
