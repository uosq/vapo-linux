#pragma once

extern "C"
{
	#include <lua5.4/lua.h>
	#include <lua5.4/lua.hpp>
	#include <lua5.4/lualib.h>
}

#define REGISTER_LUA_CONSTANT_INT(name, value) \
	lua_pushinteger(L, value); \
	lua_setglobal(L, name);

void luaregister_constants(lua_State* L);