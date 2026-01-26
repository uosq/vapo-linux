#include "classes.h"
#include "../../sdk/definitions/itexture.h"

namespace LuaClasses
{
	namespace TextureLua
	{
		const luaL_Reg methods[]
		{
			{"Delete", Delete},
			{"GetName", GetName},
			{"GetFlags", GetFlags},
			{"GetActualWidth", GetActualWidth},
			{"GetActualHeight", GetActualHeight},
			{"IsTranslucent", IsTranslucent},
			{"IsRenderTarget", IsRenderTarget},
			{nullptr, nullptr}
		};

		void luaopen_texture(lua_State* L)
		{
			luaL_newmetatable(L, "Texture");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, GC);
			lua_setfield(L, -2, "__gc");
 
			lua_pushcfunction(L, ToString);
			lua_setfield(L, -2, "__tostring");
		}

		LuaTexture* push_texture(lua_State* L, ITexture* tex)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(lua_newuserdata(L, sizeof(LuaTexture)));
			ltex->tex = tex;

			luaL_getmetatable(L, "Texture");
			lua_setmetatable(L, -2);

			return ltex;
		}

		int Index(lua_State* L)
		{
			luaL_getmetatable(L, "Texture");
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
			return 1;
		}

		int GC(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			ltex->~LuaTexture();
			return 0;
		}

		int ToString(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				lua_pushnil(L);
				return 1;
			}

			lua_pushstring(L, ltex->tex->GetName());
			return 1;
		}

		int Delete(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			ltex->tex->DecrementReferenceCount();
			ltex->tex->DeleteIfUnreferenced();
			ltex->tex = nullptr;
			return 0;
		}

		int GetName(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			lua_pushstring(L, ltex->tex->GetName());
			return 1;
		}

		int GetFlags(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			lua_pushinteger(L, ltex->tex->GetFlags());
			return 1;
		}

		int GetActualWidth(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			lua_pushinteger(L, ltex->tex->GetActualWidth());
			return 1;
		}

		int GetActualHeight(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			lua_pushinteger(L, ltex->tex->GetActualHeight());
			return 1;
		}

		int IsTranslucent(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			lua_pushboolean(L, ltex->tex->IsTranslucent());
			return 1;
		}

		int IsRenderTarget(lua_State* L)
		{
			LuaTexture* ltex = static_cast<LuaTexture*>(luaL_checkudata(L, 1, "Texture"));
			if (ltex->tex == nullptr)
			{
				luaL_error(L, "Texture is nil!");
				return 1;
			}

			lua_pushboolean(L, ltex->tex->IsRenderTarget());
			return 1;
		}
	}
}