#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pixeltypes.h"
#include "binstream.h"

class ImageCodec
{
	virtual bool write(FILE *fp, PixelArray& pb) = 0;
};

// Write out a PPM (binary) 24-bit file
class PPMBinary : public ImageCodec
{
public:
	virtual bool write(FILE * fp, PixelArray& pb)
	{
		// write out the image header
		fprintf(fp, "P6\n%d %d\n255\n", pb.width(), pb.height());

		// write the individual pixel values in binary form
		PixelRGBA *pix;
		uint8_t pixdata[3];

		for (size_t row = 0; row < pb.height(); row++)
		{
			for (size_t col = 0; col < pb.width(); col++)
			{
				pix = pb.pixelPointer(col, row);

				// Assign to array to ensure the order is
				// correct within the file
				pixdata[0] = pix->r();
				pixdata[1] = pix->g();
				pixdata[2] = pix->b();

				fwrite(pixdata, 3, 1, fp);
			}
		}

		return true;
	}
};
