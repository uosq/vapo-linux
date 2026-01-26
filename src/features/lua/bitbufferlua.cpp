#include "classes.h"
#include "../../sdk/definitions/bitbuf.h"
#include "pluto/lauxlib.h"

namespace LuaClasses
{
	namespace BitBufferLua
	{
		const luaL_Reg methods[]
		{
			{"SetCurBitPos", SetCurBitPos},
			{"GetCurBitPos", GetCurBitPos},
			{"Reset", Reset},
			{"ReadBit", ReadBit},
			{"ReadByte", ReadByte},
			{"ReadFloat", ReadFloat},
			{"ReadString", ReadString},
			{"WriteBit", WriteBit},
			{"WriteByte", WriteByte},
			{"WriteString", WriteString},
			{"WriteFloat", WriteFloat},
			{"Delete", Delete},
			{"ReadInt", ReadInt},
			{"WriteInt", WriteInt},
			{nullptr, nullptr}
		};

		void luaopen_buffer(lua_State* L)
		{
			luaL_newmetatable(L, "BitBuffer");

			luaL_setfuncs(L, methods, 0);

			lua_pushcfunction(L, Index);
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, GC);
			lua_setfield(L, -2, "__gc");

			lua_pushcfunction(L, BitBuffer);
			lua_setglobal(L, "BitBuffer");
		}

		LuaBuffer* push_buffer(lua_State* L, bf_write* writer, bf_read* reader)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(lua_newuserdata(L, sizeof(LuaBuffer)));
			lbf->writer = writer;
			lbf->reader = reader;

			luaL_getmetatable(L, "BitBuffer");
			lua_setmetatable(L, -2);

			return lbf;
		}

		int SetCurBitPos(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr || lbf->reader == nullptr)
			{
				luaL_error(L, "Expected BitBuffer, received nil");
				return 0;
			}

			if (!lua_isinteger(L, 2))
			{
				luaL_error(L, "Expected 2nd argument to be integer, received nil");
				return 0;
			}

			int bit = luaL_checkinteger(L, 2);
			lbf->curbitpos = bit;
			return 0;
		}

		int GetCurBitPos(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			lua_pushinteger(L, lbf->curbitpos);
			return 1;
		}

		int Reset(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr || lbf->reader == nullptr)
			{
				luaL_error(L, "Expected BitBuffer, received nil");
				return 0;
			}

			lbf->writer->Reset();
			lbf->reader->Reset();
			lbf->curbitpos = 0;
			return 0;
		}

		int WriteInt(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr)
			{
				luaL_error(L, "BitBuffer writer is not initialized");
				return 0;
			}

			unsigned int val = luaL_checkinteger(L, 2);
			unsigned int numbits = luaL_optinteger(L, 3, 32);

			lbf->writer->SeekToBit(lbf->curbitpos);
			lbf->writer->WriteUBitLong(val, numbits);
			
			lbf->curbitpos = lbf->writer->GetNumBitsWritten();
			return 0;
		}

		int ReadInt(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->reader == nullptr)
			{
				luaL_error(L, "BitBuffer reader is not initialized");
				return 0;
			}

			int numbits = luaL_optinteger(L, 2, 32);

			lbf->reader->Seek(lbf->curbitpos);
			int value = lbf->reader->ReadBitLong(numbits, true);

			lbf->curbitpos = lbf->reader->GetNumBitsRead();

			lua_pushinteger(L, value);
			return 1;
		}

		int WriteFloat(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr)
			{
				luaL_error(L, "BitBuffer writer is not initialized");
				return 0;
			}

			float val = luaL_checknumber(L, 2);

			lbf->writer->SeekToBit(lbf->curbitpos);
			lbf->writer->WriteFloat(val);
			
			lbf->curbitpos = lbf->writer->GetNumBitsWritten();
			return 0;
		}

		int ReadFloat(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->reader == nullptr)
			{
				luaL_error(L, "BitBuffer reader is not initialized");
				return 0;
			}

			lbf->reader->Seek(lbf->curbitpos);
			float value = lbf->reader->ReadFloat();

			lbf->curbitpos = lbf->reader->GetNumBitsRead();

			lua_pushnumber(L, value);
			return 1;
		}

		int WriteString(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr)
			{
				luaL_error(L, "BitBuffer writer is not initialized");
				return 0;
			}

			const char* val = luaL_checkstring(L, 2);

			lbf->writer->SeekToBit(lbf->curbitpos);
			lbf->writer->WriteString(val);
			
			lbf->curbitpos = lbf->writer->GetNumBitsWritten();
			return 0;
		}

		int ReadString(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->reader == nullptr)
			{
				luaL_error(L, "BitBuffer reader is not initialized");
				return 0;
			}

			int maxlength = luaL_optinteger(L, 2, 256);
			maxlength = std::clamp(maxlength, 0, 256);

			std::string str;
			str.resize(maxlength + 1);

			lbf->reader->Seek(lbf->curbitpos);
			lbf->reader->ReadString(str.data(), maxlength);

			lbf->curbitpos = lbf->reader->GetNumBitsRead();

			lua_pushstring(L, str.c_str());
			return 1;
		}

		int WriteByte(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr)
			{
				luaL_error(L, "BitBuffer writer is not initialized");
				return 0;
			}

			unsigned int val = luaL_checkinteger(L, 2);

			lbf->writer->SeekToBit(lbf->curbitpos);
			lbf->writer->WriteByte(val);
			
			lbf->curbitpos = lbf->writer->GetNumBitsWritten();
			return 0;
		}

		int ReadByte(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->reader == nullptr)
			{
				luaL_error(L, "BitBuffer reader is not initialized");
				return 0;
			}

			lbf->reader->Seek(lbf->curbitpos);
			int value = lbf->reader->ReadByte();

			lbf->curbitpos = lbf->reader->GetNumBitsRead();

			lua_pushinteger(L, value);
			return 1;
		}

		int WriteBit(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->writer == nullptr)
			{
				luaL_error(L, "BitBuffer writer is not initialized");
				return 0;
			}

			unsigned int val = luaL_checkinteger(L, 2);

			lbf->writer->SeekToBit(lbf->curbitpos);
			lbf->writer->WriteOneBit(val);
			
			lbf->curbitpos = lbf->writer->GetNumBitsWritten();
			return 0;
		}

		int ReadBit(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->reader == nullptr)
			{
				luaL_error(L, "BitBuffer reader is not initialized");
				return 0;
			}

			lbf->reader->Seek(lbf->curbitpos);
			int value = lbf->reader->ReadOneBit();

			lbf->curbitpos = lbf->reader->GetNumBitsRead();

			lua_pushinteger(L, value);
			return 1;
		}
		
		// Im not sure if im deleting them correctly
		int Delete(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			if (lbf->reader == nullptr && lbf->writer == nullptr)
			{
				luaL_error(L, "BitBuffer is nil");
				return 0;
			}

			delete lbf->writer;
			delete lbf->reader;
			delete[] lbf->data;

			lbf->reader = nullptr;
			lbf->writer = nullptr;
			lbf->data = nullptr;
			return 0;
		}

		int Index(lua_State* L)
		{
			luaL_getmetatable(L, "BitBuffer");
			lua_pushvalue(L, 2);
			lua_rawget(L, -2);
			return 1;
		}

		int GC(lua_State* L)
		{
			LuaBuffer* lbf = static_cast<LuaBuffer*>(luaL_checkudata(L, 1, "BitBuffer"));
			delete lbf->writer;
			delete lbf->reader;
			delete[] lbf->data;
			lbf->writer = nullptr;
			lbf->reader = nullptr;
			return 0;
		}

		int BitBuffer(lua_State* L)
		{
			// 1 kib should be plenty
			int bufferSize = luaL_optinteger(L, 1, 1024);
			
			LuaBuffer* lbf = static_cast<LuaBuffer*>(lua_newuserdata(L, sizeof(LuaBuffer)));

			lbf->data = new unsigned char[bufferSize];
			memset(lbf->data, 0, bufferSize);

			lbf->writer = new bf_write(lbf->data, bufferSize);
			lbf->reader = new bf_read(lbf->data, bufferSize);
			lbf->curbitpos = 0;

			luaL_getmetatable(L, "BitBuffer");
			lua_setmetatable(L, -2);
			return 1;
		}
	}
}