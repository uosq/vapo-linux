#include "netvar_parser.h"
#include <fstream>

std::vector<ClassEntry> ParseFile(const std::string& path)
{
	std::ifstream file(path);
	std::vector<ClassEntry> result;

	if (!file.is_open())
		return result;

	ClassEntry* current = nullptr;
	std::string line;

	while (std::getline(file, line))
	{
		// trim whitespace
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty())
			continue;

		// section header
		if (line.front() == '[' && line.back() == ']')
		{
			result.push_back({});
			current = &result.back();
			current->className = line.substr(1, line.size() - 2);
			continue;
		}

		if (!current)
			continue;

		// member line: class->member = 0x123
		auto arrowPos = line.find("->");
		auto eqPos    = line.find('=');

		if (arrowPos == std::string::npos)
			continue;

		std::string member;

		if (eqPos != std::string::npos)
			member = line.substr(arrowPos + 2, eqPos - arrowPos - 2);
		else
			member = line.substr(arrowPos + 2);

		// trim
		member.erase(0, member.find_first_not_of(" \t"));
		member.erase(member.find_last_not_of(" \t") + 1);

		current->members.push_back(member);
	}

	return result;
}

std::string GetThisDirectory()
{
	Dl_info info{};
	if (!dladdr((void*)&GetThisDirectory, &info) || !info.dli_fname)
		return {};

	char path[PATH_MAX];
	char* _ = realpath(info.dli_fname, path);

	// dirname may modify its argument
	return std::string(dirname(path));
}

std::string GetLocalFilePath(const std::string& filename)
{
	return GetThisDirectory() + "/" + filename;
}