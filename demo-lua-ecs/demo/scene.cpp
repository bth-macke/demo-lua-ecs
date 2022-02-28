#include "pch.h"
#include "scene.h"
#include "components.h"

void Scene::luaopen_scene(lua_State* L)
{
	// Create the 'scene' table
	lua_newtable(L);

	// Register C functions for the scene table.
	// These will need 'this' to work, so we provide
	// 'this' using upvalues.

	luaL_Reg scene_methods[] =
	{
		// { "name in lua", C function pointer }
		{ "create", CreateEntity },
		{ "destroy", DestroyEntity },
		{ "set", SetComponent },
		{ "get", GetComponent },
		{ NULL, NULL }
	};

	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "scene");
	luaL_setfuncs(L, scene_methods, 1);

	// Create the metatable that can store the
	// metamethods for the 'scene' table that
	// Lua will interact with.
	//lua_newtable(L);

	//luaL_dostring(L, "return function(t, k, v) print(\"The table can't be modified\") end");
	//lua_setfield(L, -2, "__newindex");

	//lua_pushstring(L, "Unable to access metatable.");
	//lua_setfield(L, -2, "__metatable");

	//lua_setmetatable(L, -2);


	lua_setglobal(L, "scene");
}

Scene::Scene(lua_State* L)
{
	luaopen_scene(L);
}

Scene::~Scene()
{
}

void Scene::Update()
{
}

// ---

Scene& getupvalue_scene(lua_State* L)
{
	Scene* pScene = nullptr;
	lua_getfield(L, lua_upvalueindex(1), "scene");
	if (lua_isuserdata(L, -1))
	{
		pScene = (Scene*)lua_touserdata(L, -1);
	}
	lua_pop(L, 1);
	return *pScene;
}

int Scene::CreateEntity(lua_State* L)
{
	Scene& scene = getupvalue_scene(L);

	//scene.m_registry.create();

	//printf("[C++] CreateEntity\n");
	return 0;
}

int Scene::DestroyEntity(lua_State* L)
{
	printf("[C++] DestroyEntity\n");
	return 0;
}

int Scene::SetComponent(lua_State* L)
{
	printf("[C++] SetComponent\n");
	return 0;
}

int Scene::GetComponent(lua_State* L)
{
	printf("[C++] GetComponent\n");
	return 0;
}
