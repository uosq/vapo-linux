#pragma once

#include "../definitions/fnv.h"
#include "../definitions/datatable.h"
#include <cctype>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "../interfaces/interfaces.h"
#include <iostream>
#include <fstream>
#include <iomanip>

/*
Used this tutorial for the netvar manager
https://www.youtube.com/watch?v=VCsNZ0GRVzo
*/

inline std::unordered_map<uint32_t, uint32_t> netvars;

#define NETVAR(func_name, netvar, type) type& func_name() \
{ \
	static auto offset = netvars[fnv::HashConst(netvar)]; \
	return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); \
} \

#define NETVAR_OFFSET(func_name, netvar, type, offset) type& func_name() \
{ \
	static auto m_offset = netvars[fnv::HashConst(netvar)] + offset; \
	return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + m_offset); \
} \

inline void Dump(const char* baseClass, RecvTable* table, uint32_t offset = 0)
{
	for (int i = 0; i < table->propsCount; ++i)
	{
		const auto prop = &table->props[i];
		if (!prop || std::isdigit(prop->varName[0]))
			continue;

		if (fnv::Hash(prop->varName) == fnv::HashConst("baseclass"))
			continue;

		if (prop->recvType == SendPropType::DATATABLE && prop->dataTable && prop->dataTable->tableName[0] == 'D')
			Dump(baseClass, prop->dataTable, offset + prop->offset);

		auto netvarName = baseClass + std::string("->") + prop->varName;
		netvars[fnv::Hash(netvarName.c_str())] = offset + prop->offset;
	}
}

inline void DumpToFile(
	const char* baseClass,
	RecvTable* table,
	std::ofstream& file,
	uint32_t offset = 0
)
{
	for (int i = 0; i < table->propsCount; ++i)
	{
		const auto prop = &table->props[i];
		if (!prop || std::isdigit(prop->varName[0]))
			continue;

		if (fnv::Hash(prop->varName) == fnv::HashConst("baseclass"))
			continue;

		if (prop->recvType == SendPropType::DATATABLE &&
			prop->dataTable &&
			prop->dataTable->tableName[0] == 'D')
		{
			DumpToFile(baseClass, prop->dataTable, file, offset + prop->offset);
		}

		std::string netvarName = std::string(baseClass) + "->" + prop->varName;
		uint32_t finalOffset = offset + prop->offset;

		// Store for NETVAR macro
		netvars[fnv::Hash(netvarName.c_str())] = finalOffset;

		// Dump to file
		file << netvarName
		     << " = 0x"
		     << std::hex << finalOffset
		     << std::dec << '\n';
	}
}

// Use this to dump the netvars to netvars.txt in TF2's root folder!
inline void SetupNetVarsToFile()
{
	interfaces::Cvar->ConsoleColorPrintf(
		Color_t{0, 255, 255, 255},
		"Dumping netvars...\n"
	);

	std::ofstream file("netvars.txt");
	if (!file.is_open())
	{
		interfaces::Cvar->ConsoleColorPrintf(
			Color_t{255, 0, 0, 255},
			"Failed to open netvars.txt\n"
		);
		return;
	}

	for (auto clientClass = interfaces::BaseClientDLL->GetAllClasses();
	     clientClass;
	     clientClass = clientClass->next)
	{
		if (clientClass->recvTable)
		{
			file << "\n[" << clientClass->networkName << "]\n";
			DumpToFile(clientClass->networkName, clientClass->recvTable, file);
		}
	}

	file.close();

	interfaces::Cvar->ConsoleColorPrintf(
		Color_t{150, 255, 150, 255},
		"Netvars dumped to netvars.txt\n"
	);
}

inline void SetupNetVars()
{
	//interfaces::cvar->ConsoleColorPrintf(Color_t{0, 255, 255, 255}, "Dumping netvars...\n");

	for (auto clientClass = interfaces::BaseClientDLL->GetAllClasses(); clientClass; clientClass = clientClass->next)
		// check if table valid
		if (clientClass->recvTable)
			Dump(clientClass->networkName, clientClass->recvTable);

	//interfaces::cvar->ConsoleColorPrintf(Color_t{150, 255, 150, 255}, "Netvars dumped!\n");
}