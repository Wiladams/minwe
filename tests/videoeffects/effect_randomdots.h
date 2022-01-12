#pragma once

void drawPoints(PixelMap& pmap)
{
	int rows = sqrt(maxElements);
	int cols = rows;

	int stemSize = Clamp(Map(maxElements, 1000, 200000, 30, 2), 2, 20);
	//int stemSize = 4;
	int stemHalf = stemSize / 2;

	// create circle of appriate size
	GCircle circ(stemHalf);

	// Draw some random sampled points
	for (int i = 0; i < maxElements; i++)
	{
		int x = randomRange(20, canvasWidth - 20);
		int y = randomRange(20, canvasHeight - 20);
		double u = Map(x, 0, (double)(canvasWidth - 1), 0, 1);
		double v = Map(y, 0, (double)(canvasHeight - 1), 0, 1);

		// get a random sample from the screen capture
		auto c = screenSamp->getValue(u, v, { x,y });

		// turn that into a couple of styled lines
		circ.draw(pmap, x, y, c);
	}
}
