#pragma once

#include "funcs.h"

namespace Lua
{
	extern lua_State* m_luaState;

	void InitLua();
	void CloseLua();
	void RunCode(std::string text);
}