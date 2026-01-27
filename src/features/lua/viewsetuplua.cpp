#include "classes.h"

namespace LuaClasses
{
	namespace ViewSetupLua
	{
		void luaopen_viewsetup(lua_State* L)
		{
			luaL_newmetatable(L, "ViewSetup");

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, NewIndex);
			lua_setfield(L, -2, "__newindex");
		}

		LuaViewSetup* push_viewsetup(lua_State* L, CViewSetup* view)
		{
			LuaViewSetup* lview = static_cast<LuaViewSetup*>(lua_newuserdata(L, sizeof(LuaViewSetup)));
			lview->view = view;

			luaL_getmetatable(L, "ViewSetup");
			lua_setmetatable(L, -2);

			return lview;
		}

		int Index(lua_State* L)
		{
			LuaViewSetup* lview = static_cast<LuaViewSetup*>(luaL_checkudata(L, 1, "ViewSetup"));

			const char* key = luaL_checkstring(L, 2);
			if (strcmp(key, "x") == 0) { lua_pushinteger(L, lview->view->x); return 1; }
			if (strcmp(key, "m_nUnscaledX") == 0) { lua_pushinteger(L, lview->view->m_nUnscaledX); return 1; }
			if (strcmp(key, "y") == 0) { lua_pushinteger(L, lview->view->y); return 1; }
			if (strcmp(key, "m_nUnscaledY") == 0) { lua_pushinteger(L, lview->view->m_nUnscaledY); return 1; }
			if (strcmp(key, "width") == 0) { lua_pushinteger(L, lview->view->width); return 1; }
			if (strcmp(key, "m_nUnscaledWidth") == 0) { lua_pushinteger(L, lview->view->m_nUnscaledWidth); return 1; }
			if (strcmp(key, "height") == 0) { lua_pushinteger(L, lview->view->height); return 1; }
			if (strcmp(key, "m_nUnscaledHeight") == 0) { lua_pushinteger(L, lview->view->m_nUnscaledHeight); return 1; }
			if (strcmp(key, "m_bOrtho") == 0) { lua_pushnumber(L, lview->view->m_bOrtho); return 1; }
			if (strcmp(key, "m_OrthoLeft") == 0) { lua_pushnumber(L, lview->view->m_OrthoLeft); return 1; }
			if (strcmp(key, "m_OrthoTop") == 0) { lua_pushnumber(L, lview->view->m_OrthoTop); return 1; }
			if (strcmp(key, "m_OrthoRight") == 0) { lua_pushnumber(L, lview->view->m_OrthoRight); return 1; }
			if (strcmp(key, "m_OrthoBottom") == 0) { lua_pushnumber(L, lview->view->m_OrthoBottom); return 1; }
			if (strcmp(key, "fov") == 0) { lua_pushnumber(L, lview->view->fov); return 1; }
			if (strcmp(key, "fovViewmodel") == 0) { lua_pushnumber(L, lview->view->fovViewmodel); return 1; }
			if (strcmp(key, "origin") == 0) { LuaClasses::VectorLua::push_vector(L, lview->view->origin); return 1; }
			if (strcmp(key, "angles") == 0) { LuaClasses::VectorLua::push_vector(L, lview->view->angles); return 1; }
			if (strcmp(key, "zNear") == 0) { lua_pushnumber(L, lview->view->zNear); return 1; }
			if (strcmp(key, "zFar") == 0) { lua_pushnumber(L, lview->view->zFar); return 1; }
			if (strcmp(key, "zNearViewmodel") == 0) { lua_pushnumber(L, lview->view->zNearViewmodel); return 1; }
			if (strcmp(key, "zFarViewmodel") == 0) { lua_pushnumber(L, lview->view->zFarViewmodel); return 1; }
			if (strcmp(key, "m_flAspectRatio") == 0) { lua_pushnumber(L, lview->view->m_flAspectRatio); return 1; }
			
			lua_pushnil(L);
			return 1;
		}

		int NewIndex(lua_State* L)
		{
			LuaViewSetup* lview = static_cast<LuaViewSetup*>(luaL_checkudata(L, 1, "ViewSetup"));

			const char* key = luaL_checkstring(L, 2);
			if (strcmp(key, "x") == 0) { lview->view->x = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "m_nUnscaledX") == 0) { lview->view->m_nUnscaledX = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "y") == 0) { lview->view->y = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "m_nUnscaledY") == 0) { lview->view->m_nUnscaledY = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "width") == 0) { lview->view->width = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "m_nUnscaledWidth") == 0) { lview->view->m_nUnscaledWidth = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "height") == 0) { lview->view->height = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "m_nUnscaledHeight") == 0) { lview->view->m_nUnscaledHeight = luaL_checkinteger(L, 3); return 1; }
			if (strcmp(key, "m_bOrtho") == 0) { lview->view->m_bOrtho = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "m_OrthoLeft") == 0) { lview->view->m_OrthoLeft = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "m_OrthoTop") == 0) { lview->view->m_OrthoTop = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "m_OrthoRight") == 0) { lview->view->m_OrthoRight = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "m_OrthoBottom") == 0) { lview->view->m_OrthoBottom = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "fov") == 0) { lview->view->fov = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "fovViewmodel") == 0) { lview->view->fovViewmodel = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "origin") == 0) { lview->view->origin = *static_cast<Vector*>(luaL_checkudata(L, 3, "Vector3")); return 1; }
			if (strcmp(key, "angles") == 0) { lview->view->angles = *static_cast<Vector*>(luaL_checkudata(L, 3, "Vector3")); return 1; }
			if (strcmp(key, "zNear") == 0) { lview->view->zNear = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "zFar") == 0) { lview->view->zFar = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "zNearViewmodel") == 0) { lview->view->zNearViewmodel = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "zFarViewmodel") == 0) { lview->view->zFarViewmodel = luaL_checknumber(L, 3); return 1; }
			if (strcmp(key, "m_flAspectRatio") == 0) { lview->view->m_flAspectRatio = luaL_checknumber(L, 3); return 1; }
			
			lua_pushnil(L);
			return 1;
		}
	}
}