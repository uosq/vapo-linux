#pragma once

#include <string>
#include <unordered_map>
#include <vector>

extern "C"
{
	#include <lua5.4/lua.h>
	#include "lua5.4/lua.hpp"
	#include <lua5.4/lualib.h>
}

struct LuaHook
{
	std::string name;
	int lua_func_ref;
};

namespace LuaHookManager
{
	extern std::unordered_map<std::string, std::vector<LuaHook>> hooks;

	void Add(std::string name, std::string id, int lua_func_ref);
	bool Remove(lua_State* L, std::string name, std::string id);
	void Call(lua_State* L, const std::string& name, int nargs = 0, bool popargs = true);
}