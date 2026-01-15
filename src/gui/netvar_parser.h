#pragma once

#include <string>
#include <vector>

#include <dlfcn.h>
#include <limits.h>
#include <string>
#include <libgen.h>

struct ClassEntry
{
	std::string className;
	std::vector<std::string> members;
};

std::vector<ClassEntry> ParseFile(const std::string& path);
std::string GetThisDirectory();
std::string GetLocalFilePath(const std::string& filename);