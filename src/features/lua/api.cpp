#include "api.h"
#include "../../gui/console.h"
#include "classes.h"
#include "funcs.h"
#include "constants.h"

namespace Lua
{
	lua_State* m_luaState = nullptr;

	void InitLua()
	{
		m_luaState = luaL_newstate();
		luaL_openlibs(m_luaState);

		LuaFuncs::common::luaopen_commonfunctions(m_luaState);
		LuaFuncs::globalvars::luaopen_globalvars(m_luaState);
		LuaFuncs::hooks::luaopen_hooks(m_luaState);
		LuaFuncs::engine::luaopen_engine(m_luaState);
		LuaFuncs::entitylist::luaopen_entitylist(m_luaState);
		LuaFuncs::draw::luaopen_draw(m_luaState);

		LuaClasses::VectorLua::luaopen_vector(m_luaState);
		LuaClasses::EntityLua::luaopen_entity(m_luaState);

		luaregister_constants(m_luaState);

		consoleText += "Lua initialized\n";
	}

	void CloseLua()
	{
		lua_close(m_luaState);
	}

	void RunCode(std::string text)
	{
		if (m_luaState == nullptr)
		{
			consoleText += "Lua state is nil!\n";
			return;
		}

		if (luaL_dostring(m_luaState, text.c_str()) != LUA_OK)
		{
			const char* err = luaL_tolstring(m_luaState, -1, nullptr);
			if (err)
				consoleText += std::string(err) + "\n";
			lua_pop(m_luaState, 1);
		}
	}
}