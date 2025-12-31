#pragma once

#include <cstddef>
#include <cstdint>
#include <sys/mman.h>
#include <unistd.h>

namespace vtable
{
	inline void** get(void* obj)
	{
		return *reinterpret_cast<void***>(obj);
	}

	template <typename T>
	inline T hook(void** table, int index, T detour)
	{
		size_t page = sysconf(_SC_PAGESIZE);
		uintptr_t start = (uintptr_t)table & ~(page - 1);

		mprotect((void*)start, page, PROT_READ | PROT_WRITE);
		T orig = (T)table[index];
		table[index] = (void*)detour;
		mprotect((void*)start, page, PROT_READ);

		return orig;
	}

	inline uintptr_t ResolveRIP(uintptr_t instr, int dispOffset, int instrSize)
	{
		int32_t disp = *reinterpret_cast<int32_t*>(instr + dispOffset);
		return instr + instrSize + disp;
	}

	template <size_t I, typename T, typename... Args>
	inline T call(void* p, Args... args)
	{
		auto vTable = *static_cast<void***>(p);
		return reinterpret_cast<T(*)(void*, Args...)>(vTable[I])(p, args...);
	}

	template <size_t I, typename T, typename... Args>
	inline T call(uintptr_t u, Args... args)
	{
		auto p = reinterpret_cast<void*>(u);
		auto vTable = *static_cast<void***>(p);
		return reinterpret_cast<T(*)(void*, Args...)>(vTable[I])(p, args...);
	}
}
