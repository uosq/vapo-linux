#include "classes.h"

#include <string>
#include "../../sdk/math.h"
#include <cstring>

namespace LuaClasses
{
	namespace VectorLua
	{
		const luaL_Reg methods[] =
		{
			{"Set", Set},
			{"Normalize", Normalize},
			{"Length", Length},
			{"Length2D", Length2D},
			{"LengthSqr", LengthSqr},
			{"Length2DSqr", Length2DSqr},
			{"Cross", Cross},
			{"Dot", Dot},
			{"DistTo", DistTo},
			{"DistTo2D", DistTo2D},
			{"ToAngle", ToAngle},
			{"Angles", Angles},
			{"ToString", ToString},
			{nullptr, nullptr},
		};

		void luaopen_vector(lua_State* L)
		{
			luaL_newmetatable(L, "Vector3");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, NewIndex);
			lua_setfield(L, -2, "__newindex");

			lua_pushcfunction(L, GC);
			lua_setfield(L, -2, "__gc");

			lua_pushcfunction(L, ToString);
			lua_setfield(L, -2, "__tostring");

			lua_pushcfunction(L, Vector3);
			lua_setglobal(L, "Vector3");
		}

		int Vector3(lua_State* L)
		{
			lua_Number x = luaL_optnumber(L, 1, 0.0);
			lua_Number y = luaL_optnumber(L, 2, 0.0);
			lua_Number z = luaL_optnumber(L, 3, 0.0);

			Vector* v = static_cast<Vector*>(lua_newuserdata(L, sizeof(Vector)));
			//new (v) Vector((float)x, (float)y, (float)z);
			v->Set(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

			luaL_getmetatable(L, "Vector3");
			lua_setmetatable(L, -2);

			return 1;
		}

		int GC(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			v->~Vector();
			return 0;
		}

		Vector* push_vector(lua_State* L, float x, float y, float z)
		{
			Vector* v = static_cast<Vector*>(lua_newuserdata(L, sizeof(Vector)));
			v->Set(x, y, z);

			luaL_getmetatable(L, "Vector3");
			lua_setmetatable(L, -2);

			return v;
		}

		Vector* push_vector(lua_State* L, Vector vec)
		{
			Vector* v = static_cast<Vector*>(lua_newuserdata(L, sizeof(Vector)));
			new (v) Vector(vec.x, vec.y, vec.z);

			luaL_getmetatable(L, "Vector3");
			lua_setmetatable(L, -2);

			return v;
		}

		int Index(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			const char* key = luaL_checkstring(L, 2);

			if (strcmp(key, "x") == 0) { lua_pushnumber(L, v->x); return 1; }
			if (strcmp(key, "y") == 0) { lua_pushnumber(L, v->y); return 1; }
			if (strcmp(key, "z") == 0) { lua_pushnumber(L, v->z); return 1; }

			// fallback to methods
			luaL_getmetatable(L, "Vector3");
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
			return 1;
		}

		int NewIndex(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			const char* key = luaL_checkstring(L, 2);
			float value = (float)luaL_checknumber(L, 3);

			if (strcmp(key, "x") == 0) { v->x = value; return 0; }
			if (strcmp(key, "y") == 0) { v->y = value; return 0; }
			if (strcmp(key, "z") == 0) { v->z = value; return 0; }

			return luaL_error(L, "invalid field '%s'", key);
		}

		int ToString(lua_State* L)
		{
			Vector* v = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			std::string str = (std::to_string(v->x) + ", " + std::to_string(v->y) + ", " + std::to_string(v->z));
			lua_pushlstring(L, str.c_str(), str.size());
			return 1;
		}

		int Set(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float x = 0, y = 0, z = 0;

			x = static_cast<float>(luaL_checknumber(L, 2));
			y = static_cast<float>(luaL_checknumber(L, 3));
			z = static_cast<float>(luaL_checknumber(L, 4));

			vec->Set(x, y, z);
			return 0;
		}

		int Normalize(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Normalize();
			lua_pushnumber(L, length);
			return 1;
		}

		int Length(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Length();
			lua_pushnumber(L, length);
			return 1;
		}

		int Length2D(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Length2D();
			lua_pushnumber(L, length);
			return 1;
		}

		int LengthSqr(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->LengthSqr();
			lua_pushnumber(L, length);
			return 1;
		}

		int Length2DSqr(lua_State* L)
		{
			Vector* vec = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			float length = vec->Length2DSqr();
			lua_pushnumber(L, length);
			return 1;
		}

		int Cross(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector cross = a->Cross(*b);
			push_vector(L, cross);
			return 1;
		}

		int Dot(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector dot = a->Dot(*b);
			push_vector(L, dot);
			return 1;
		}

		int DistTo(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector dist = a->DistTo(*b);
			push_vector(L, dist);
			return 1;
		}

		int DistTo2D(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector* b = static_cast<Vector*>(luaL_checkudata(L, 2, "Vector3"));
			Vector dist = a->DistTo2D(*b);
			push_vector(L, dist);
			return 1;
		}

		int ToAngle(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));
			Vector angle = a->ToAngle();
			push_vector(L, angle);
			return 1;
		}

		int Angles(lua_State* L)
		{
			Vector* a = static_cast<Vector*>(luaL_checkudata(L, 1, "Vector3"));

			Vector forward, right, up;
			Math::AngleVectors(*a, &forward, &right, &up);

			push_vector(L, forward);
			push_vector(L, right);
			push_vector(L, up);
			return 1;
		}
	}
}