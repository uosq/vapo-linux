#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "pluto/lua.hpp"
#include "pluto/lualib.h"

struct LuaHook
{
	std::string name;
	int lua_func_ref;
};

namespace LuaHookManager
{
	extern std::unordered_map<std::string, std::vector<LuaHook>> hooks;

	bool Add(std::string name, std::string id, int lua_func_ref);
	bool Remove(lua_State* L, std::string name, std::string id);
	void Call(lua_State* L, const std::string& name, int nargs = 0, bool popargs = true);
	bool HasHooks(const std::string& name);
}