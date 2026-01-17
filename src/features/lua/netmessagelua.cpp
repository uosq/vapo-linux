#include "classes.h"

extern "C"
{
	#include <lua5.4/lua.h>
	#include <lua5.4/lua.hpp>
	#include <lua5.4/lualib.h>
}


namespace LuaClasses
{
	namespace NetMessageLua
	{
		const luaL_Reg methods[]
		{
			{"GetType", GetType},
			{"GetName", GetName},
			{"ToString", ToString},
			{"WriteToBitBuffer", WriteToBitBuffer},
			{"ReadFromBitBuffer", ReadFromBitBuffer},
			{"IsReliable", IsReliable},
			{"SetReliable", SetReliable},
			{"GetGroup", GetGroup},
			{nullptr, nullptr}
		};

		void luaopen_netmessage(lua_State* L)
		{
			luaL_newmetatable(L, "NetMessage");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, GC);
			lua_setfield(L, -2, "__gc");

			lua_pushcfunction(L, ToString);
			lua_setfield(L, -2, "__tostring");
		}

		LuaNetMessage* push_netmessage(lua_State* L, INetMessage* msg)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(lua_newuserdata(L, sizeof(LuaNetMessage)));
			lnm->msg = msg;

			luaL_getmetatable(L, "NetMessage");
			lua_setmetatable(L, -2);
			return lnm;
		}

		int GetType(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushinteger(L, lnm->msg->GetType());
			return 1;
		}

		int GetName(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushstring(L, lnm->msg->GetName());
			return 1;
		}

		int ToString(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushstring(L, lnm->msg->ToString());
			return 1;
		}

		int WriteToBitBuffer(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				luaL_error(L, "Expected NetMessage, received nil");
				return 0;
			}

			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 2, "BitBuffer"));
			if (lbf->writer == nullptr)
			{
				luaL_error(L, "Expected 2nd argument to be BitBuffer, received nil");
				return 0;
			}

			// wtf?
			lnm->msg->WriteToBuffer(*lbf->writer);
			return 0;
		}

		int ReadFromBitBuffer(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				luaL_error(L, "Expected NetMessage, received nil");
				return 0;
			}

			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 2, "BitBuffer"));
			if (lbf->reader == nullptr)
			{
				luaL_error(L, "Expected 2nd argument to be BitBuffer, received nil");
				return 0;
			}

			lnm->msg->ReadFromBuffer(*lbf->reader);
			return 0;
		}

		int IsReliable(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				luaL_error(L, "Expected NetMessage, received nil");
				return 0;
			}

			lua_pushboolean(L, lnm->msg->IsReliable());
			return 1;
		}

		int SetReliable(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				luaL_error(L, "Expected NetMessage, received nil");
				return 0;
			}

			int reliable = luaL_checkinteger(L, 1);
			lnm->msg->SetReliable(reliable);
			return 0;
		}

		int GetGroup(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			if (lnm->msg == nullptr)
			{
				luaL_error(L, "Expected NetMessage, received nil");
				return 0;
			}

			lua_pushinteger(L, lnm->msg->GetGroup());
			return 1;
		}

		int GC(lua_State* L)
		{
			LuaNetMessage* lnm = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			lnm->msg = nullptr;
			lnm->~LuaNetMessage();
			return 0;
		}

		int Index(lua_State* L)
		{
			LuaNetMessage* v = static_cast<LuaNetMessage*>(luaL_checkudata(L, 1, "NetMessage"));
			luaL_getmetatable(L, "NetMessage");
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
			return 1;
		}
	}
}