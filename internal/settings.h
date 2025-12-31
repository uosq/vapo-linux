#pragma once

#include "json.hpp"

struct Settings_ESP {
	bool enabled = true;
	bool ignorecloaked = true;
};

struct Settings
{
	Settings_ESP esp;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings_ESP, enabled)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, esp)

static inline Settings settings;