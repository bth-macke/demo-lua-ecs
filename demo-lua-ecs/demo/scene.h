#include "pch.h"

class Scene
{
	entt::registry m_registry;

private:

	void luaopen_scene(lua_State* L);

public:

	Scene(lua_State* L);
	~Scene();

	void Update();

public:

	// Lua C API
	static int CreateEntity(lua_State* L);
	static int DestroyEntity(lua_State* L);
	static int SetComponent(lua_State* L);
	static int GetComponent(lua_State* L);
};