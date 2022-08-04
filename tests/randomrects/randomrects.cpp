

#include "apphost.h"
#include "draw.h"
#include "maths.hpp"

using namespace maths;

bool outlineOnly = false;


INLINE PixelRGBA randomColor(uint32_t alpha=255)
{
	uint32_t r = random_int(255);
	uint32_t g = random_int(255);
	uint32_t b = random_int(255);

	return { r,g,b,alpha };
}

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();

	if (e.keyCode == VK_SPACE)
		outlineOnly = !outlineOnly;
}

void onLoop()
{
	PixelRGBA stroke;
	PixelRGBA fill;
	PixelRGBA c;

	gAppSurface->setAllPixels(PixelRGBA(0x0));

	for (int i = 1; i <= 2000; i++)
	{
		int x1 = random_int(canvasWidth - 1);
		int y1 = random_int(canvasHeight - 1);
		int lwidth = random_int(4, 60);
		int lheight = random_int(4, 60);

		c = randomColor(192);

		if (outlineOnly)
		{
			stroke = c;
			draw::rectangle_copy(*gAppSurface, x1, y1, lwidth, lheight, c);

		}
		else
		{
			fill = c;
			//draw::rectangle_copy(*gAppSurface, x1, y1, lwidth, lheight, c);
			draw::rectangle_blend(*gAppSurface, x1, y1, lwidth, lheight, c);
		}
	}

	refreshScreen();
}


void onLoad()
{
	setCanvasSize(800, 800);

	//windowOpacity(0.33);

	//gAppWindow->moveTo(0, 0);
	//layered();
}
