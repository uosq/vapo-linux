#pragma once

#include <vector>
#include "../definitions/isurface.h"
#include "../interfaces/interfaces.h"

struct FontManager
{
	HFont CreateFont(const char* name, int height, int weight, int flags = 0)
	{
		HFont font = interfaces::surface->CreateFont();
		interfaces::surface->SetFontGlyphSet(font, name, height, weight, 0, 0, flags);
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
		CreateFont("Arial", 16, 400, EFONTFLAG_CUSTOM | EFONTFLAG_ANTIALIAS);
	}

	private:
	std::vector<HFont> fonts;
	HFont current = 0;
};

static inline FontManager fontManager;