#include "pch.h"

struct NameComponent
{
	std::string Name;
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

// ---

struct ScriptComponent
{
	std::string ScriptPath;
};
