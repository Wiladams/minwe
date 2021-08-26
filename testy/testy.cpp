#include "apphost.h"
#include <stdio.h>

void onLoad()
{
	puts("onLoad()  :-)");
}

void onUnload()
{
	puts("onUnload() :-(");
}

void onLoop()
{
	static int count = 0;
	count = count + 1;
	printf("Loop: %d\n", count);
}