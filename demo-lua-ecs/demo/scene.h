#include "pch.h"

class Scene
{
	entt::registry m_registry;
	lua_State* m_luaState;

private:

	void luaopen_scene(lua_State* L);

public:

	Scene(lua_State* L);
	~Scene();

	void Update();

public:

	// Scene API
	static int CreateEntity(lua_State* L);
	static int DestroyEntity(lua_State* L);

	// Entity API
	static int SetComponent(lua_State* L);
	static int GetComponent(lua_State* L);
};