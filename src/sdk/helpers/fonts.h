#pragma once

#include <vector>
#include "../definitions/isurface.h"
#include "../interfaces/interfaces.h"

struct FontManager
{
	HFont CreateFont(const char* name, int height, int weight, int flags = EFONTFLAG_ANTIALIAS | EFONTFLAG_CUSTOM)
	{
		HFont font = interfaces::Surface->CreateFont();
		interfaces::Surface->SetFontGlyphSet(font, name, height, weight, 0, 0, flags);
		fonts.push_back(font);
		current = font;
		return font;
	}

	HFont GetFont(int index)
	{
		return fonts[index];
	}

	HFont GetCurrentFont()
	{
		return current;
	}
	
	void SetCurrentFont(HFont font)
	{
		current = font;
	}

	void Init()
	{
		CreateFont("Verdana", 16, 400, EFONTFLAG_CUSTOM | EFONTFLAG_ANTIALIAS | EFONTFLAG_OUTLINE);
	}

	private:
	std::vector<HFont> fonts;
	HFont current = 0;
};

static FontManager fontManager;