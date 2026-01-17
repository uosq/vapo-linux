#include "../../sdk/definitions/imaterial.h"
#include "classes.h"

extern "C"
{
	#include <lua5.4/lua.h>
	#include <lua5.4/lua.hpp>
	#include <lua5.4/lualib.h>
}

namespace LuaClasses
{
	namespace MaterialLua
	{
		const luaL_Reg methods[]
		{
			{"SetColorModulation", SetColorModulation},
			{"GetColorModulation", GetColorModulation},
			{"SetMaterialVarFlag", SetMaterialVarFlag},
			{"GetMaterialVarFlag", GetMaterialVarFlag},
			{"GetTextureGroupName", GetTextureGroupName},
			{"GetName", GetName},
			{nullptr, nullptr}
		};

		void luaopen_material(lua_State* L)
		{
			luaL_newmetatable(L, "Material");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, GC);
			lua_setfield(L, -2, "__gc");
 
			lua_pushcfunction(L, ToString);
			lua_setfield(L, -2, "__tostring");
		}

		int Index(lua_State* L)
		{
			luaL_getmetatable(L, "Material");
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
			return 1;
		}

		int GC(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			lmat->mat->Release();
			lmat->mat->DeleteIfUnreferenced();
			return 0;
		}

		int ToString(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
			{
				lua_pushstring(L, "Nil material");
				return 1;
			}

			const char* name = lmat->mat->GetName();
			lua_pushlstring(L, name, sizeof(name));
			return 1;
		}

		LuaMaterial* push_material(lua_State* L, IMaterial* mat)
		{
			LuaMaterial* v = static_cast<LuaMaterial*>(lua_newuserdata(L, sizeof(LuaMaterial)));
			v->mat = mat;

			luaL_getmetatable(L, "Material");
			lua_setmetatable(L, -2);

			return v;
		}

		int SetColorModulation(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
				return 0;

			float r = luaL_optnumber(L, 2, 0.0f);
			float g = luaL_optnumber(L, 3, 0.0f);
			float b = luaL_optnumber(L, 4, 0.0f);

			r = std::clamp(r, 0.0f, 1.0f);
			g = std::clamp(g, 0.0f, 1.0f);
			b = std::clamp(b, 0.0f, 1.0f);

			lmat->mat->ColorModulate(r, g, b);
			return 0;
		}

		int GetColorModulation(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
			{
				lua_pushnil(L);
				lua_pushnil(L);
				lua_pushnil(L);
				return 3;
			}

			float r, g, b;
			lmat->mat->GetColorModulation(&r, &g, &b);

			lua_pushnumber(L, r);
			lua_pushnumber(L, g);
			lua_pushnumber(L, b);

			return 3;
		}

		int SetMaterialVarFlag(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
				return 0;

			MaterialVarFlags_t flag = static_cast<MaterialVarFlags_t>(luaL_checkinteger(L, 2));
			int on = luaL_checkinteger(L, 2);

			lmat->mat->SetMaterialVarFlag(flag, on);
			return 0;
		}

		int GetMaterialVarFlag(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
				return 0;

			MaterialVarFlags_t flag = static_cast<MaterialVarFlags_t>(luaL_checkinteger(L, 2));
			bool on = lmat->mat->GetMaterialVarFlag(flag);

			lua_pushboolean(L, on);
			return 1;
		}

		int GetTextureGroupName(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
				return 0;

			const char* groupName = lmat->mat->GetTextureGroupName();
			lua_pushlstring(L, groupName, sizeof(groupName));
			return 1;
		}

		int GetName(lua_State* L)
		{
			LuaMaterial* lmat = static_cast<LuaMaterial*>(luaL_checkudata(L, 1, "Material"));
			if (lmat->mat == nullptr)
				return 0;

			const char* name = lmat->mat->GetName();
			lua_pushlstring(L, name, sizeof(name));
			return 1;
		}
	}
}