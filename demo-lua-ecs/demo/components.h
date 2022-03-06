#include "pch.h"

struct NameComponent
{
	std::string Name;
};

void luaC_pushname(lua_State* L, const NameComponent& component);
NameComponent luaC_toname(lua_State* L, int i);

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

void luaC_pushtransform(lua_State* L, const TransformComponent& component);
TransformComponent luaC_totransform(lua_State* L, int i);

// ---

struct ScriptComponent
{
	std::string ScriptPath;
	int EnvReference = 0;
};

void luaC_pushscript(lua_State* L, const ScriptComponent& component);
ScriptComponent luaC_toscript(lua_State* L, int i);
