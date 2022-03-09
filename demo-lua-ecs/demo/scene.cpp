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
		{ "CreateEntity", CreateEntity },
		{ "DestroyEntity", DestroyEntity },
		{ NULL, NULL }
	};

	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "scene");
	luaL_setfuncs(L, scene_methods, 1);

	lua_setglobal(L, "scene");

	// Create a metatable for entities. This will create
	// a metatable that is not exposed to the Lua environment.
	luaL_newmetatable(L, "entityMeta");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_Reg entity_methods[] =
	{
		// { "name in lua", C function pointer }
		{ "SetComponent", SetComponent },
		{ "GetComponent", GetComponent },
		{ NULL, NULL }
	};

	// Set- and GetComponent both need the Entt::registry to work.
	// We store this scene object as an upvalue for these C functions.
	// Create a table for the upvalues. This is not necessary, but can
	// come handy if we want to add more data for these functions.
	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "scene");
	luaL_setfuncs(L, entity_methods, 1);
	lua_setmetatable(L, -2);
}

Scene::Scene(lua_State* L) : m_luaState(L)
{
	luaopen_scene(m_luaState);
}

Scene::~Scene()
{
}

void Scene::Update()
{
	auto scripts = m_registry.view<ScriptComponent>();
	scripts.each([&](ScriptComponent& script) {
		
		// Push the entity table to the stack
		lua_rawgeti(m_luaState, LUA_REGISTRYINDEX, script.LuaEntityRef);

		// Call the entitys 'OnUpdate' method, passing
		// the table as 'self'-argument
		lua_getfield(m_luaState, -1, "OnUpdate");
		lua_pushvalue(m_luaState, -2);
		if (lua_pcall(m_luaState, 1, 0, 0) != LUA_OK)
		{
			luaC_dumpError(m_luaState);
		}

		// Pop the entity table
		lua_pop(m_luaState, 1);
	});
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
	// Retrieve arguments and pop them from the stack
	std::string entityFile = lua_tostring(L, -1);
	lua_pop(L, 1);


	// Retrieve the scene, stores an an upvalue for
	// this function
	Scene& scene = getupvalue_scene(L);

	// Load and run the entity script. The script
	// has to return a table that will end up on
	// top of the stack.
	luaL_loadfile(L, entityFile.c_str());
	if (lua_pcall(L, 0, 1, 0) != LUA_OK)
	{
		luaC_dumpError(L);
	}

	lua_pushvalue(L, -1);
	int luaEntityRef = luaL_ref(L, LUA_REGISTRYINDEX);

	// Give the entity table a handle to entt
	entt::entity entity = scene.m_registry.create();
	auto script = scene.m_registry.emplace<ScriptComponent>(entity, entityFile, luaEntityRef);

	lua_pushinteger(L, (int)entity);
	lua_setfield(L, -2, "handle");

	luaL_getmetatable(L, "entityMeta");
	lua_setmetatable(L, -2);

	lua_getfield(L, -1, "OnStart");
	lua_pushvalue(L, -2);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaC_dumpError(L);
	}

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

//int Scene::SetName(lua_State* L)
//{
//	entt::entity entity = (entt::entity)lua_tointeger(L, 1);
//
//	NameComponent component;
//	component.Name = lua_tostring(L, 2);
//
//	Scene& scene = getupvalue_scene(L);
//	scene.m_registry.emplace_or_replace<NameComponent>(entity, component);
//	printf("[C++] Set name \"%s\" for entity %i\n", component.Name.c_str(), (int)entity);
//
//	return 0;
//}
//
//int Scene::SetTransform(lua_State* L)
//{
//	entt::entity entity = (entt::entity)lua_tointeger(L, 1);
//
//	TransformComponent component;
//
//	lua_geti(L, 2, 1);
//	lua_geti(L, 2, 2);
//	lua_geti(L, 2, 3);
//	component.Position.x = lua_tonumber(L, -3);
//	component.Position.y = lua_tonumber(L, -2);
//	component.Position.z = lua_tonumber(L, -1);
//	lua_pop(L, 3);
//
//	lua_geti(L, 3, 1);
//	lua_geti(L, 3, 2);
//	lua_geti(L, 3, 3);
//	component.Rotation.x = lua_tonumber(L, -3);
//	component.Rotation.y = lua_tonumber(L, -2);
//	component.Rotation.z = lua_tonumber(L, -1);
//	lua_pop(L, 3);
//
//	lua_geti(L, 4, 1);
//	lua_geti(L, 4, 2);
//	lua_geti(L, 4, 3);
//	component.Scale.x = lua_tonumber(L, -3);
//	component.Scale.y = lua_tonumber(L, -2);
//	component.Scale.z = lua_tonumber(L, -1);
//	lua_pop(L, 3);
//
//	Scene& scene = getupvalue_scene(L);
//	scene.m_registry.emplace_or_replace<TransformComponent>(entity, component);
//	printf("[C++] Set transform for entity %i\n", (int)entity);
//	printf("  Position:  %f, %f, %f\n", component.Position.x, component.Position.y, component.Position.z);
//	printf("  Rotation:  %f, %f, %f\n", component.Rotation.x, component.Rotation.y, component.Rotation.z);
//	printf("  Scale:     %f, %f, %f\n", component.Scale.x, component.Scale.y, component.Scale.z);
//
//	return 0;
//}
//
//int Scene::SetScript(lua_State* L)
//{
//	// Store arguments and pop them from the stack
//	entt::entity entity = (entt::entity)lua_tointeger(L, 1);
//	ScriptComponent component;
//	component.ScriptPath = lua_tostring(L, 2);
//	lua_pop(L, 2);
//
//	// Retrieve the scene, stores an an upvalue for
//	// this function
//	Scene& scene = getupvalue_scene(L);
//
//	// Create a script component for the entity
//	scene.m_registry.emplace_or_replace<ScriptComponent>(entity, component);
//
//	// Run the script file to load the entity table
//	luaL_loadfile(L, component.ScriptPath.c_str());
//	if (lua_pcall(L, 0, 1, 0) != LUA_OK)
//	{
//		luaC_dumpError(L);
//	}
//
//	// Create a metatable for entity
//	lua_newtable(L);
//	lua_pushinteger(L, (int)entity);
//	lua_setfield(L, -2, "handle");
//
//	lua_pushvalue(L, -1);
//	lua_setfield(L, -2, "__index");
//
//	lua_setmetatable(L, -2);
//	
//	// Check if the table has the method "Start"
//
//	lua_getfield(L, -1, "Start");
//	lua_pushvalue(L, -2);
//
//	// Run the method "Start"
//	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
//	{
//		luaC_dumpError(L);
//	}
//
//	int size = lua_gettop(L);
//
//	// Return the entity table
//	return 1;
//}
