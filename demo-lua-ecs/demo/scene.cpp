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
		{ "setName", SetName },
		{ "setTransform", SetTransform },
		{ "setScript", SetScript },
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

	entt::entity entity = scene.m_registry.create();

	lua_pushinteger(L, (int)entity);
	printf("[C++] Created entity %i\n", (int)entity);
	
	return 1;
}

int Scene::DestroyEntity(lua_State* L)
{
	entt::entity entity = (entt::entity)lua_tointeger(L, 1);
	lua_pop(L, 1);

	Scene& scene = getupvalue_scene(L);
	scene.m_registry.destroy(entity);
	printf("[C++] Created entity %i\n", (int)entity);

	return 0;
}

int Scene::SetName(lua_State* L)
{
	entt::entity entity = (entt::entity)lua_tointeger(L, 1);

	NameComponent component;
	component.Name = lua_tostring(L, 2);

	Scene& scene = getupvalue_scene(L);
	scene.m_registry.emplace_or_replace<NameComponent>(entity, component);
	printf("[C++] Set name \"%s\" for entity %i\n", component.Name.c_str(), (int)entity);

	return 0;
}

int Scene::SetTransform(lua_State* L)
{
	entt::entity entity = (entt::entity)lua_tointeger(L, 1);

	TransformComponent component;

	lua_geti(L, 2, 1);
	lua_geti(L, 2, 2);
	lua_geti(L, 2, 3);
	component.Position.x = lua_tonumber(L, -3);
	component.Position.y = lua_tonumber(L, -2);
	component.Position.z = lua_tonumber(L, -1);
	lua_pop(L, 3);

	lua_geti(L, 3, 1);
	lua_geti(L, 3, 2);
	lua_geti(L, 3, 3);
	component.Rotation.x = lua_tonumber(L, -3);
	component.Rotation.y = lua_tonumber(L, -2);
	component.Rotation.z = lua_tonumber(L, -1);
	lua_pop(L, 3);

	lua_geti(L, 4, 1);
	lua_geti(L, 4, 2);
	lua_geti(L, 4, 3);
	component.Scale.x = lua_tonumber(L, -3);
	component.Scale.y = lua_tonumber(L, -2);
	component.Scale.z = lua_tonumber(L, -1);
	lua_pop(L, 3);

	Scene& scene = getupvalue_scene(L);
	scene.m_registry.emplace_or_replace<TransformComponent>(entity, component);
	printf("[C++] Set transform for entity %i\n", (int)entity);
	printf("  Position:  %f, %f, %f\n", component.Position.x, component.Position.y, component.Position.z);
	printf("  Rotation:  %f, %f, %f\n", component.Rotation.x, component.Rotation.y, component.Rotation.z);
	printf("  Scale:     %f, %f, %f\n", component.Scale.x, component.Scale.y, component.Scale.z);

	return 0;
}

int Scene::SetScript(lua_State* L)
{
	entt::entity entity = (entt::entity)lua_tointeger(L, 1);

	ScriptComponent component;
	component.ScriptPath = lua_tostring(L, 2);

	lua_pop(L, 2);

	int size = lua_gettop(L);

	// Create a new environment for the entity
	lua_newtable(L);
	lua_pushinteger(L, (int)entity);
	lua_setfield(L, -2, "handle");

	lua_newtable(L);
	lua_getglobal(L, "_G");
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);

	size = lua_gettop(L);

	// Pops the table
	component.EnvReference = luaL_ref(L, LUA_REGISTRYINDEX);

	size = lua_gettop(L);

	Scene& scene = getupvalue_scene(L);
	scene.m_registry.emplace_or_replace<ScriptComponent>(entity, component);

	// If the script has OnAttach function, retrieve the entitys
	// environment and call OnAttach.

	luaL_loadfile(L, component.ScriptPath.c_str());

	lua_rawgeti(L, LUA_REGISTRYINDEX, component.EnvReference);
	lua_setupvalue(L, 1, 1);

	size = lua_gettop(L);
	if (lua_pcall(L, 0, 1, 0) != LUA_OK)
	{
		luaC_dumpError(L);
	}
	size = lua_gettop(L);

	lua_rawgeti(L, LUA_REGISTRYINDEX, component.EnvReference);
	lua_pushvalue(L, 1);
	lua_setfield(L, 2, "entity");
	lua_pop(L, 1);
	size = lua_gettop(L);

	lua_getglobal(L, "_ENV");
	lua_rawgeti(L, LUA_REGISTRYINDEX, component.EnvReference);
	lua_setglobal(L, "_ENV");
	size = lua_gettop(L);
	
	lua_getfield(L, 1, "OnAttach");
	lua_pushvalue(L, 1);
	size = lua_gettop(L);

	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaC_dumpError(L);
	}
	size = lua_gettop(L);

	lua_setglobal(L, "_ENV");
	size = lua_gettop(L);

	return 1;
}
