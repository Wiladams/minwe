#include "gui.h"
#include "textlayout.h"

TextLayout tLayout;

void drawAlignedText()
{
	int midx = canvasWidth / 2;
	int midy = canvasHeight / 2;

	// draw vertical line down center of canvas
	line(*gAppSurface, midx, 0, midx, canvasHeight - 1, PixelRGBA(0xff000000));

	// draw horizontal line across canvas
	line(*gAppSurface, 0, midy, canvasWidth - 1, midy, PixelRGBA(0xff000000));

	tLayout.textFont("Consolas", 24);
	tLayout.textColor(PixelRGBA(0xff000000));

	tLayout.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
	tLayout.text("LEFT", midx, 24);

	tLayout.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
	tLayout.text("CENTER", midx, 48);

	tLayout.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::BASELINE);
	tLayout.text("RIGHT", midx, 72);

	tLayout.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::BASELINE);
	tLayout.text("SOUTH EAST", midx, midy);

	tLayout.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
	tLayout.text("SOUTH WEST", midx, midy);

	tLayout.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
	tLayout.text("NORTH EAST", midx, midy);

	tLayout.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
	tLayout.text("NORTH WEST", midx, midy);
}


void setup()
{
	setCanvasSize(320, 320);

	tLayout.init(gAppSurface);

	background(PixelRGBA(0xffDDDDDD));

	drawAlignedText();
}