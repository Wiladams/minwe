#include "apphost.h"
#include "ticktopic.h"

/*
	An example of using the TickTopic

	There are two topics here.  One generates events telling
	us when the screen should be refreshed: gTickRefresh
	The other tells us when to draw: gTickTopic

	These two topics are independent, and not coordinated in 
	any way.  You will notice the appearance of drawing will be
	quite choppy.  You can play with how out of sync things
	can be by adjusting the frequencies of the two topics
	to different values.

	To create a more orderly drawing, either a single topic
	can be used, where you get periodic events, do the drawing
	and refresh to screen all at once.  Or, you can provide 
	other synchronization mechanisms outside the topics.
*/
TickTopic gTickDraw(30);
TickTopic gTickRefresh(10);

// Respond to ticks telling us it's time to draw
void drawFrame()
{
	static int line = 0;

	//printf("frameTicker: %3.2f\n", t);

	// draw a red line across 
	gAppSurface->hline(0, line, canvasWidth, rgb(255, 0, 0));

	// increment position
	line = line + 1;
	if (line >= canvasHeight)
	{
		gAppSurface->setAllPixels(rgb(255, 255, 255));
		line = 0;
	}
}
static void drawTicker(const Topic<double>& p, const double t)
{
	drawFrame();
}

// Respond to tick events indicating time for frame refresh
static void refreshTicker(const Topic<double>& p, const double t)
{
	drawFrame();
	screenRefresh();
}

// Testing out the timer topic
void onLoad()
{
	// clear screen to white
	gAppSurface->setAllPixels(rgb(255, 255, 255));

	// Setup frame refresh 
	//gTickRefresh.setFrequency(15);
	gTickRefresh.subscribe(refreshTicker);
	gTickRefresh.start();

	// Setup frame drawing
	//gTickDraw.setFrequency(15);
	//gTickDraw.subscribe(drawTicker);
	//gTickDraw.start();

}