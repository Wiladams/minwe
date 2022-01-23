#include "apphost.h"
#include "maths.hpp"
#include "pixeltypes.h"

void onLoad()
{
	// create a couple of texels
	TexelRect t1(0, 0, 0.499, 1);
	TexelRect t2(0.5, 0, 1, 1);
		
	int MAXCOUNT = 10;

	for (int count=1; count <= MAXCOUNT; count++)
	{
		double u = maths::Map(count, 1, MAXCOUNT, 0, 1);

		TexelRect aRect = maths::Lerp(u, t1, t2);

		printf("(%3.2f, %3.2f, %3.2f, %3.2f)\n", aRect.left, aRect.top, aRect.right, aRect.bottom);
	}
}