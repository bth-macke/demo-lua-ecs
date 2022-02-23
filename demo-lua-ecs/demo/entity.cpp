#include "entity.h"

int luaopen_entity(lua_State* L)
{
	lua_newtable(L);

	lua_pushstring(L, "Marcus");
	lua_setfield(L, -2, "name");

	lua_setglobal(L, "entity");
	return 0;
}
