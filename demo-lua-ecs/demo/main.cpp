#include "pch.h"

// Lua initialize functions
void luaopen_entity(lua_State* L, entt::registry& registry);
void luaopen_component(lua_State* L, entt::registry& registry);

// C helper functions
entt::registry& luaC_getupvalue_registry(lua_State* L);

// C functions exposed to Lua
int EntityCreate(lua_State* L);
int EntitySet(lua_State* L);
int EntityGet(lua_State* L);

// Component types

// !!! Not visible in Lua! Stores info for
// Lua entities.
struct ScriptComponent
{
	std::string InitScriptPath;
	int LuaEntityRef = 0;
};

struct TransformComponent
{
	struct PositionData
	{
		float x = 0;
		float y = 0;
		float z = 0;
	} Position;

	struct EulerRotationData
	{
		float x = 0;
		float y = 0;
		float z = 0;
	} Rotation;

	struct ScaleData
	{
		float x = 0;
		float y = 0;
		float z = 0;
	} Scale;
};

// Component functions
void luaC_pushtransform(lua_State* L, const TransformComponent& component);
TransformComponent luaC_totransform(lua_State* L, int i);



int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	entt::registry registry;
	luaopen_entity(L, registry);

	std::string chunk;
	while (chunk != "exit") {
		std::cout << "> ";
		std::getline(std::cin, chunk);

		int result = luaL_dostring(L, chunk.c_str());

		if (chunk == "clear") {
			system("cls");
		}
		else if (chunk == "exit") {
			std::cout << "Bye bye!" << std::endl;
		}
		else if (chunk == "update") {
			auto scripts = registry.view<ScriptComponent>();
			scripts.each([&](ScriptComponent& script) {

				// Push the entity table to the stack
				lua_rawgeti(L, LUA_REGISTRYINDEX, script.LuaEntityRef);

				// Call the entitys 'OnUpdate' method, passing
				// the table as 'self'-argument
				lua_getfield(L, -1, "OnUpdate");
				lua_pushvalue(L, -2);
				if (lua_pcall(L, 1, 0, 0) != LUA_OK)
				{
					luaC_dumpError(L);
				}

				// Pop the entity table
				lua_pop(L, 1);
			});
		}
		else if (result != LUA_OK) {
			luaC_dumpError(L);
		}
	}

	return 0;
}



// ######################################################################### //
// ######################## Lua initialize functions ####################### //
// ######################################################################### //

void luaopen_entity(lua_State* L, entt::registry& registry)
{
	// Create a metatable for Lua entities. This table will
	// store all C functions that the Lua entities need
	// access to.
	luaL_newmetatable(L, "entitymeta");

	// If a field in is not found in the Lua entity table,
	// make Lua search into this metatable instead. This
	// enables all Lua entities to have access to the set
	// and get methods.
	lua_pushvalue(L, -1); // Pushes copy of the metatable
	lua_setfield(L, -2, "__index");

	// These will need the entt::registry to work, so we
	// provide it using upvalues. The registry is placed
	// as a "lightuserdata" in a table. The table can be
	// expanded with more values in the future, but we
	// only need one upvalue if everything is stored in
	// a single table.
	luaL_Reg entity_methods[] =
	{
		// { "name in lua", C function pointer }
		{ "create", EntityCreate },
		{ "set", EntitySet },
		{ "get", EntityGet },
		{ NULL, NULL }
	};

	// These will need the entt::registry to work, so we
	// provide it using upvalues (same as above).
	lua_newtable(L);
	lua_pushlightuserdata(L, &registry);
	lua_setfield(L, -2, "registry");
	luaL_setfuncs(L, entity_methods, 1);

	// Expose the metatable to the Lua environment.
	// This is not secure as Lua may override this
	// table, but we'll do it for flexibility.
	lua_setglobal(L, "entity");
}

void luaopen_component(lua_State* L, entt::registry& registry)
{
	{ // Transform
		luaL_newmetatable(L, "entitymeta");
		lua_pushvalue(L, -1); // Pushes copy of the metatable
		lua_setfield(L, -2, "__index");
	}
}

// ######################################################################### //
// ########################### C helper functions ########################## //
// ######################################################################### //

entt::registry& luaC_getupvalue_registry(lua_State* L)
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



// ######################################################################### //
// ########################## Exposed C functions ########################## //
// ######################################################################### //

/// <summary>
///		Creates a new entity in the registry and returns a table that represents
///		the entity in the Lua environment.
/// </summary>
/// 
/// <param name="L">
///		Stack:
///		1 | string entityFilePath | -1
/// </param>
/// 
/// <returns>
///		A table that represents the entity in the Lua environment.
/// </returns>
int EntityCreate(lua_State* L)
{
	// Retrieve arguments and pop them from the stack
	std::string initFile = lua_tostring(L, -1);
	lua_pop(L, 1);

	entt::registry& registry = luaC_getupvalue_registry(L);

	// Load the entitys setup script and run it. The
	// script has to return a table that will end up
	// on top of the stack.
	luaL_loadfile(L, initFile.c_str());
	if (lua_pcall(L, 0, 1, 0) != LUA_OK)
	{
		luaC_dumpError(L);
	}
	// Note: the new entitys table now exist on top
	// of the stack!

	// In order to access the Lua entity from C, we
	// store a ref that can be used to retrieve it in
	// in the future.
	lua_pushvalue(L, -1);
	int luaEntityRef = luaL_ref(L, LUA_REGISTRYINDEX);

	// Now we have everything we need to create an
	// entity in the C environment. We store script
	// relatable data in a ScriptComponent.
	entt::entity entity = registry.create();
	auto script = registry.emplace<ScriptComponent>(entity, initFile, luaEntityRef);

	// Let the Lua entity store the entt identifier.
	lua_pushinteger(L, (int)entity);
	lua_setfield(L, -2, "id");

	// Assign the entity metatable, giving the Lua
	// entity access to get/set methods for components.
	luaL_getmetatable(L, "entitymeta");
	lua_setmetatable(L, -2);

	// The Lua entity is now finished. Let's call its
	// "OnCreate" method if it exists.
	// The Lua entity (table) is needed as the self
	// argument.
	lua_getfield(L, -1, "OnCreate");
	lua_pushvalue(L, -2);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK)
	{
		luaC_dumpError(L);
	}

	return 1;
}

/// <summary>
///		Creates or replaces a component for an entity.
/// </summary>
/// 
/// <param name="L">
///		Stack:
///		3...N | <variadic component args>
///		    2 | string componentType
///		    1 | table entity
/// </param>
/// 
/// <returns>
///		None
/// </returns>
int EntitySet(lua_State* L)
{
	entt::registry& registry = luaC_getupvalue_registry(L);

	// Retrieve arguments and pop them from the stack
	lua_getfield(L, 1, "id");
	entt::entity entity = (entt::entity)lua_tointeger(L, -1);
	lua_pop(L, 1); // Pop the "id" value

	std::string type = lua_tostring(L, 2);

	if (type == "transform")
	{
		TransformComponent transform = luaC_totransform(L, 3);
		registry.emplace_or_replace<TransformComponent>(entity, transform);
	}
	// Pop entity, type, and component data
	// Not really necessary as we return 0 anyway.
	lua_pop(L, 3);

	return 0;
}

/// <summary>
///		Retrieves a component of an entity if it exists.
/// </summary>
/// 
/// <param name="L">
///		Stack:
///		    2 | string componentType    | -1
///		    1 | table entity            | -2
/// </param>
/// 
/// <returns>
///		A table representing the requested component of
///		the entity, or nil if the component don't exists.
/// </returns>
int EntityGet(lua_State* L)
{
	entt::registry& registry = luaC_getupvalue_registry(L);

	// Retrieve arguments and pop them from the stack
	lua_getfield(L, 1, "id");
	entt::entity entity = (entt::entity)lua_tointeger(L, -1);
	std::string type = lua_tostring(L, 2);
	lua_pop(L, 3); // Pop entity, type, and id.

	// Note: The stack is now empty.

	if (type == "transform")
	{
		auto& transform = registry.get<TransformComponent>(entity);
		luaC_pushtransform(L, transform);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

// ######################################################################### //
// ########################## Component functions ########################## //
// ######################################################################### //

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
	lua_geti(L, -1, 1); component.Position.x = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_geti(L, -1, 2); component.Position.y = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_geti(L, -1, 3); component.Position.z = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_pop(L, 1); // Pop position

	lua_getfield(L, -1, "rotation");
	lua_geti(L, -1, 1); component.Rotation.x = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_geti(L, -1, 2); component.Rotation.y = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_geti(L, -1, 3); component.Rotation.z = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_pop(L, 1); // Pop rotation

	lua_getfield(L, -1, "scale");
	lua_geti(L, -1, 1); component.Scale.x = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_geti(L, -1, 2); component.Scale.y = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_geti(L, -1, 3); component.Scale.z = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_pop(L, 1); // Pop scale
	lua_pop(L, 1); // Pop transform

	return component;
}
