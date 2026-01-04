#pragma once

#include <string>
#include "../sdk/helpers/helper.h"

inline bool _dropDownOpen = false;

struct BaseStyle
{
	Color background{46, 52, 64, 255};
	Color textcolor{255, 255, 255, 255};
	Color border{136, 192, 208, 255};
};

struct WindowContext
{
	bool dragging = false;
	int oldMouseX = 0;
	int oldMouseY = 0;
	int x = 0;
	int y = 0;
	int headerHeight = 20;
	int width = 0, height = 0;
};

inline bool _IsMouseInside(int x, int y, int width, int height)
{
	int mouseX, mouseY;
	interfaces::Surface->SurfaceGetCursorPos(mouseX, mouseY);
	return mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height);
}

inline void _DragWindow(WindowContext &ctx, int mouseX, int mouseY)
{
	bool isDown = interfaces::InputSystem->IsButtonDown(MOUSE_LEFT);
	if (!isDown)
	{
		ctx.dragging = false;
		return;
	}

	int hx = ctx.x;
	int hy = ctx.y - ctx.headerHeight;

	int tickpressed = interfaces::InputSystem->GetButtonPressedTick(MOUSE_LEFT);

	if (tickpressed >= interfaces::GlobalVars->tickcount && isDown && _IsMouseInside(hx, hy, ctx.width, ctx.headerHeight))
		ctx.dragging = true;

	if (ctx.dragging)
	{
		ctx.x += (mouseX - ctx.oldMouseX);
		ctx.y += (mouseY - ctx.oldMouseY);
	}
}

inline void GUI_Window(WindowContext &ctx, BaseStyle &style, std::string title = "")
{
	int mouseX, mouseY;
	interfaces::Surface->SurfaceGetCursorPos(mouseX, mouseY);

	_DragWindow(ctx, mouseX, mouseY);

	int x, y, width, height;
	x = ctx.x;
	y = ctx.y;
	width = ctx.width;
	height = ctx.height;

	{	// background
		helper::draw::SetColor(style.background);
		helper::draw::FilledRect(x, y, x + width, y + height);
	}

	{	// border
		helper::draw::SetColor(style.border);
		helper::draw::OutlinedRect(x - 1, y - 1, x + width + 1, y + height + 1);
	}

	{	// header
		int hx = ctx.x;
		int hy = ctx.y - ctx.headerHeight;

		helper::draw::SetColor(style.border);
		helper::draw::FilledRect(hx, hy, hx + width + 2, hy + ctx.headerHeight + 1);

		{	// title
			int textw, texth;
			helper::draw::GetTextSize(title, textw, texth);
	
			int textx, texty;
			textx = x + (int)(width*0.5f) - (int)(textw*0.5f);
			texty = hy + (int)(ctx.headerHeight*0.5f) - (int)(texth*0.5f);
			helper::draw::Text(textx, texty, style.textcolor, title);
		}
	}

	ctx.oldMouseX = mouseX;
	ctx.oldMouseY = mouseY;
}

inline void GUI_InitWindow(WindowContext &ctx, int x, int y, int w, int h, int headerHeight = 20)
{
	ctx.x = x;
	ctx.y = y;
	ctx.width = w;
	ctx.height = h;
	ctx.headerHeight = headerHeight;
}