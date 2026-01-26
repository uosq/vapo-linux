#include "api.h"
#include "../../gui/console.h"
#include "classes.h"
#include "funcs.h"
#include "constants.h"

namespace Lua
{
	lua_State* m_luaState = nullptr;

	void InitPluto()
	{
		m_luaState = luaL_newstate();
		luaL_openlibs(m_luaState);

		LuaFuncs::common::luaopen_commonfunctions(m_luaState);

		// open libraries
		LuaFuncs::globalvars::luaopen_globalvars(m_luaState);
		LuaFuncs::hooks::luaopen_hooks(m_luaState);
		LuaFuncs::engine::luaopen_engine(m_luaState);
		LuaFuncs::entitylist::luaopen_entitylist(m_luaState);
		LuaFuncs::draw::luaopen_draw(m_luaState);
		LuaFuncs::render::luaopen_render(m_luaState);
		LuaFuncs::materials::luaopen_materials(m_luaState);
		LuaFuncs::client::luaopen_client(m_luaState);
		LuaFuncs::clientstate::luaopen_clientstate(m_luaState);
		LuaFuncs::input::luaopen_input(m_luaState);
		LuaFuncs::menu::luaopen_menu(m_luaState);
		LuaFuncs::ui::luaopen_ui(m_luaState);
		LuaFuncs::aimbot::luaopen_aimbot(m_luaState);

		// open classes
		LuaClasses::VectorLua::luaopen_vector(m_luaState);
		LuaClasses::EntityLua::luaopen_entity(m_luaState);
		LuaClasses::MaterialLua::luaopen_material(m_luaState);
		LuaClasses::BitBufferLua::luaopen_buffer(m_luaState);
		LuaClasses::NetMessageLua::luaopen_netmessage(m_luaState);
		LuaClasses::TextureLua::luaopen_texture(m_luaState);
		LuaClasses::NetChannelLua::luaopen_netchannel(m_luaState);
		LuaClasses::GameEventLua::luaopen_gameevent(m_luaState);

		luaregister_constants(m_luaState);

		consoleText += "Pluto initialized\n";
	}

	void ClosePluto()
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