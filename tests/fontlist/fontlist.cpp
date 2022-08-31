#include "gui.h"
#include "textlayout.h"
#include "fontmonger.h"

#include <list>

std::list<std::string> fontList;
TextLayout tLayout;

void drawFonts()
{
	constexpr int rowHeight = 24;
	constexpr int colWidth = 213;

	int maxRows = canvasHeight / rowHeight;
	int maxCols = canvasWidth / colWidth;

	int col = 0;
	int row = 0;

	std::list<std::string>::iterator it;
	for (it = fontList.begin(); it != fontList.end(); ++it) 
	{
		int x = col * colWidth;
		int y = row * rowHeight;

		tLayout.textFont(it->c_str(), 18);
		tLayout.text(it->c_str(), x, y);

		col++;
		if (col >= maxCols)
		{
			col = 0;
			row++;
		}
	}

}

void setup()
{
	setCanvasSize(1280, 1024);

	FontMonger::collectFontFamilies(fontList);
	tLayout.init(gAppSurface);

	//FontMonger::listFontFamilies();

	background(PixelRGBA(0xffdcdcdc));
	//textColor(PixelRGBA(0xffffffff));

	drawFonts();
}
