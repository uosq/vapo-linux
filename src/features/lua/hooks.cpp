#include "hooks.h"
#include "../../gui/console.h"

namespace LuaHookManager
{
    std::unordered_map<std::string, std::vector<LuaHook>> hooks;

	void Add(std::string name, std::string id, int lua_func_ref)
	{
		auto& eventHooks = hooks[name];
		for (const auto& hook : eventHooks)
		{
			if (hook.name == id)
				return; 
		}

		eventHooks.push_back({ id, lua_func_ref });
	}

	bool Remove(lua_State* L, std::string name, std::string id)
	{
		auto it = hooks.find(name);
		if (it == hooks.end()) return false;

		auto& vec = it->second;
		for (auto vec_it = vec.begin(); vec_it != vec.end(); ++vec_it)
		{
			if (vec_it->name == id)
			{
				luaL_unref(L, LUA_REGISTRYINDEX, vec_it->lua_func_ref);
				
				vec.erase(vec_it);
				return true;
			}
		}
		return false;
	}

	void Call(lua_State* L, const std::string& name, int nargs, bool popargs)
	{
		auto it = hooks.find(name);
		if (it == hooks.end()) 
		{
			if (nargs > 0 && popargs)
				lua_pop(L, nargs);
			return;
		}

		const auto& eventHooks = it->second;
		if (eventHooks.empty())
		{
			if (nargs > 0 && popargs)
				lua_pop(L, nargs);
			return;
		}

		int base = lua_gettop(L) - nargs + 1;

		for (const auto& hook : eventHooks)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, hook.lua_func_ref);
			
			if (nargs > 0)
			{
				for (int i = 0; i < nargs; ++i)
				{
					lua_pushvalue(L, base + i);
				}
			}
			
			if (lua_pcall(L, nargs, 0, 0) != LUA_OK)
			{
				const char* error = lua_tostring(L, -1);
				consoleText += std::string(error) + "\n";
				lua_pop(L, 1); // pop error message
			}
		}

		if (nargs > 0 && popargs)
			lua_pop(L, nargs);
	}
}