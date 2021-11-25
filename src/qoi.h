#pragma once

/*

QOI - The “Quite OK Image” format for fast, lossless image compression

Dominic Szablewski - https://phoboslab.org


-- LICENSE: The MIT License(MIT)

Copyright(c) 2021 Dominic Szablewski

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


-- About

QOI encodes and decodes images in a lossless format. An encoded QOI image is
usually around 10--30% larger than a decently optimized PNG image.

QOI outperforms simpler PNG encoders in compression ratio and performance. QOI
images are typically 20% smaller than PNGs written with stbi_image but 10%
larger than with libpng. Encoding is 25-50x faster and decoding is 3-4x faster
than stbi_image or libpng.


-- Synopsis

// Define `QOI_IMPLEMENTATION` in *one* C/C++ file before including this
// library to create the implementation.

#define QOI_IMPLEMENTATION
#include "qoi.h"

// Load and decode a QOI image from the file system into a 32bbp RGBA buffer
int width, height;
void *rgba_pixels = qoi_read("image.qoi", &width, &height, 4);

// Encode and store an RGBA buffer to the file system
qoi_write("image_new.qoi", rgba_pixels, width, height, 4);


-- Documentation

This library provides the following functions;
- qoi_read    -- read and decode a QOI file
- qoi_decode  -- decode the raw bytes of a QOI image from memory
- qoi_write   -- encode and write a QOI file
- qoi_encode  -- encode an rgba buffer into a QOI image in memory

See the function declaration below for the signature and more information.

If you don't want/need the qoi_read and qoi_write functions, you can define
QOI_NO_STDIO before including this library.

This library uses malloc() and free(). To supply your own malloc implementation
you can define QOI_MALLOC and QOI_FREE before including this library.


-- Data Format

A QOI file has the following header, followed by any number of data "chunks".

struct qoi_header_t {
	char [4];              // magic bytes "qoif"
	unsigned short width;  // image width in pixels
	unsigned short height; // image height in pixels
	unsigned int size;     // number of data bytes following this header
};

The decoder and encoder start with {r: 0, g: 0, b: 0, a: 255} as the previous
pixel value. Pixels are either encoded as
 - a run of the previous pixel
 - an index into a previously seen pixel
 - a difference to the previous pixel value in r,g,b,a
 - full r,g,b,a values

A running array[64] of previously seen pixel values is maintained by the encoder
and decoder. Each pixel that is seen by the encoder and decoder is put into this
array at the position (r^g^b^a) % 64. In the encoder, if the pixel value at this
index matches the current pixel, this index position is written to the stream.

Each chunk starts with a 2, 3 or 4 bit tag, followed by a number of data bits.
The bit length of chunks is divisible by 8 - i.e. all chunks are byte aligned.

QOI_INDEX {
	u8 tag  :  2;   // b00
	u8 idx  :  6;   // 6-bit index into the color index array: 0..63
}

QOI_RUN_8 {
	u8 tag  :  3;   // b010
	u8 run  :  5;   // 5-bit run-length repeating the previous pixel: 1..32
}

QOI_RUN16 {
	u8 tag  :  3;   // b011
	u16 run : 13;   // 13-bit run-length repeating the previous pixel: 33..8224
}

QOI_DIFF8 {
	u8 tag  :  2;   // b10
	u8 dr   :  2;   // 2-bit   red channel difference: -1..2
	u8 dg   :  2;   // 2-bit green channel difference: -1..2
	u8 db   :  2;   // 2-bit  blue channel difference: -1..2
}

QOI_DIFF16 {
	u8 tag  :  3;   // b110
	u8 dr   :  5;   // 5-bit   red channel difference: -15..16
	u8 dg   :  4;   // 4-bit green channel difference:  -7.. 8
	u8 db   :  4;   // 4-bit  blue channel difference:  -7.. 8
}

QOI_DIFF24 {
	u8 tag  :  4;   // b1110
	u8 dr   :  5;   // 5-bit   red channel difference: -15..16
	u8 dg   :  5;   // 5-bit green channel difference: -15..16
	u8 db   :  5;   // 5-bit  blue channel difference: -15..16
	u8 da   :  5;   // 5-bit alpha channel difference: -15..16
}

QOI_COLOR {
	u8 tag  :  4;   // b1111
	u8 has_r:  1;   //   red byte follows
	u8 has_g:  1;   // green byte follows
	u8 has_b:  1;   //  blue byte follows
	u8 has_a:  1;   // alpha byte follows
	u8 r;           //   red value if has_r == 1: 0..255
	u8 g;           // green value if has_g == 1: 0..255
	u8 b;           //  blue value if has_b == 1: 0..255
	u8 a;           // alpha value if has_a == 1: 0..255
}

*/

/*
	Note: William A Adams
	The original functions assumed writing to a file.  I want to work with 
	streams instead, so binstream is used.

	Also, a chunk is allocated for every frame encoded.  I want to change
	this to using a pre-allocated chunk (the stream) so we save on 
	alloc/free for every frame.
*/
// -----------------------------------------------------------------------------
// Header - Public functions

#include "binstream.h"
#include "pixelmap.h"

#ifndef QOI_MALLOC
#define QOI_MALLOC(sz) malloc(sz)
#define QOI_FREE(p)    free(p)
#endif

#define QOI_INDEX   0x00 // 00xxxxxx
#define QOI_RUN_8   0x40 // 010xxxxx
#define QOI_RUN_16  0x60 // 011xxxxx
#define QOI_DIFF_8  0x80 // 10xxxxxx
#define QOI_DIFF_16 0xc0 // 110xxxxx
#define QOI_DIFF_24 0xe0 // 1110xxxx
#define QOI_COLOR   0xf0 // 1111xxxx

#define QOI_MASK_2  0xc0 // 11000000
#define QOI_MASK_3  0xe0 // 11100000
#define QOI_MASK_4  0xf0 // 11110000



typedef union {
	struct { unsigned char r, g, b, a; } rgba;
	unsigned int v;
} qoi_rgba_t;

typedef union {
	char chars[4];
	unsigned int v;
} qoi_magic_t;

typedef struct {
	qoi_magic_t magic;
	unsigned short width;
	unsigned short height;
	unsigned int size;
} qoi_header_t;

const static qoi_magic_t qoi_magic = { .chars = {'q','o','i','f'} };

static inline unsigned int QOI_COLOR_HASH(qoi_rgba_t C) {
	return (C.rgba.r ^ C.rgba.g ^ C.rgba.b ^ C.rgba.a);
}

class QIOCodec 
{
public:
	static int maxBufferSize(const int w, const int h, const int channels) {
		int max_size = w * h * (channels + 1) + sizeof(qoi_header_t) + 4;
		return max_size;
	}


	// Encode raw RGB or RGBA pixels into a QOI image in memory. w and h denote the
	// width and height of the pixel data. channels must be either 3 for RGB data 
	// or 4 for RGBA.
	// The function either returns NULL on failure (invalid parameters or malloc 
	// failed) or a pointer to the encoded data on success. On success the out_len
	// is set to the size in bytes of the encoded data.
	// The returned qoi data should be free()d after user.
	//
	// WAA - Assume the stream is already capable of handling maxBufferSize() amount
	// of data
	static bool encode(BinStream &bs, const void* data, int w, int h, int channels, int* out_len) 
		{
			// Quick reject if parameters are not correct
			if (
				data == NULL || out_len == NULL ||
				w <= 0 || w >= (1 << 16) ||
				h <= 0 || h >= (1 << 16) ||
				channels < 3 || channels > 4
				) {
				return false;
			}


			// Write the header
			bs.writeBytes(qoi_magic.chars, 4);
			bs.writeUInt16(w);
			bs.writeUInt16(h);
			// Where is the size field?
			int sizeStart = bs.tell();
			bs.writeUInt32(0);	// combe back to this later, take sentinel

			int dataStart = bs.tell();

			const unsigned char* pixels = (const unsigned char*)data;

			qoi_rgba_t index[64] = { 0 };

			int run = 0;
			qoi_rgba_t px_prev = { .rgba = {.r = 0, .g = 0, .b = 0, .a = 255} };
			qoi_rgba_t px = px_prev;

			int px_len = w * h * channels;
			int px_end = px_len - channels;
			for (int px_pos = 0; px_pos < px_len; px_pos += channels) {
				if (channels == 4) {
					px = *(qoi_rgba_t*)(pixels + px_pos);
				}
				else {
					px.rgba.r = pixels[px_pos];
					px.rgba.g = pixels[px_pos + 1];
					px.rgba.b = pixels[px_pos + 2];
				}

				if (px.v == px_prev.v) {
					run++;
				}

				if (run > 0 && (run == 0x2020 || px.v != px_prev.v || px_pos == px_end)) {
					if (run < 33) {
						run -= 1;
						bs.writeOctet(QOI_RUN_8 | run);
					}
					else {
						run -= 33;
						bs.writeOctet(QOI_RUN_16 | run >> 8);
						bs.writeOctet(run);
					}
					run = 0;
				}

				if (px.v != px_prev.v) {
					int index_pos = QOI_COLOR_HASH(px) % 64;

					if (index[index_pos].v == px.v) {
						bs.writeOctet(QOI_INDEX | index_pos);
					}
					else {
						index[index_pos] = px;

						int vr = px.rgba.r - px_prev.rgba.r;
						int vg = px.rgba.g - px_prev.rgba.g;
						int vb = px.rgba.b - px_prev.rgba.b;
						int va = px.rgba.a - px_prev.rgba.a;

						if (
							vr > -16 && vr < 17 && vg > -16 && vg < 17 &&
							vb > -16 && vb < 17 && va > -16 && va < 17
							) {
							if (
								va == 0 && vr > -2 && vr < 3 &&
								vg > -2 && vg < 3 && vb > -2 && vb < 3
								) {
								bs.writeOctet(QOI_DIFF_8 | ((vr + 1) << 4) | (vg + 1) << 2 | (vb + 1));
							}
							else if (
								va == 0 && vr > -16 && vr < 17 &&
								vg > -8 && vg < 9 && vb > -8 && vb < 9
								) {
								bs.writeOctet(QOI_DIFF_16 | (vr + 15));
								bs.writeOctet(((vg + 7) << 4) | (vb + 7));
							}
							else {
								bs.writeOctet(QOI_DIFF_24 | ((vr + 15) >> 1));
								bs.writeOctet((vr + 15) << 7) | ((vg + 15) << 2) | ((vb + 15) >> 3);
								bs.writeOctet(((vb + 15) << 5) | (va + 15));
							}
						}
						else {
							bs.writeOctet(QOI_COLOR | (vr ? 8 : 0) | (vg ? 4 : 0) | (vb ? 2 : 0) | (va ? 1 : 0));
							if (vr) { bs.writeOctet(px.rgba.r); }
							if (vg) { bs.writeOctet(px.rgba.g); }
							if (vb) { bs.writeOctet(px.rgba.b); }
							if (va) { bs.writeOctet(px.rgba.a); }
						}
					}
				}
				px_prev = px;
			}

			for (int i = 0; i < 4; i++) {
				bs.writeOctet(0);
			}

			// Figure out the size of the data written
			int dataEnd = bs.tell();
			int dataSize = dataEnd - dataStart;

			// go back and write that size into the stream
			bs.seek(sizeStart);
			bs.writeUInt32(dataSize);

			// Reset the stream to where we stopped
			// writing data
			bs.seek(dataEnd);

			*out_len = dataSize;

			return true;
		}

	// Decode a QOI image from memory into either raw RGB (channels=3) or RGBA 
	// (channels=4) pixel data.
	// The function either returns NULL on failure (invalid parameters or malloc 
	// failed) or a pointer to the decoded pixels. On success out_w and out_h will
	// be set to the width and height of the decoded image.
	// The returned pixel data should be free()d after use.

		static bool decode(BinStream& bs, PixelMap& pmap)
		{
			if (bs.remaining() < sizeof(qoi_header_t))
			{
				return false;
			}

			// Read in the header
			qoi_header_t header;
			bs.readBytes((uint8_t*)header.magic.chars, 4);
			header.width = bs.readUInt16();
			header.height = bs.readUInt16();
			header.size = bs.readUInt32();
			int channels = 4;

			//header.size + sizeof(qoi_header_t) != size ||
			if ((channels < 3) ||
				(channels > 4) ||
				(header.width != pmap.width()) ||
				(header.height != pmap.height()) ||
				(header.magic.v != qoi_magic.v)
				)
			{
				return false;
			}

			// how many bytes are needed to hold pixel data

			int px_len = header.width * header.height * channels;
			if (px_len > bs.remaining()) {
				return false;
			}

			// We're going to decode directly into the pixel pointer
			// of the PixelMap
			unsigned char* pixels = (unsigned char*)pmap.getPixelPointer(0, 0);

			// bs is the stream from which we are reading
			// encoded data
			//const unsigned char* bytes = (const unsigned char*)data + sizeof(qoi_header_t);

			int data_len = header.size;
			qoi_rgba_t px = { .rgba = {.r = 0, .g = 0, .b = 0, .a = 255} };
			qoi_rgba_t index[64] = { 0 };

			int run = 0;
			for (int px_pos = 0; px_pos < px_len && bs.remaining() > 0; px_pos += channels)
			{
				if (run > 0) {
					run--;
				}
				else {
					int b1 = bs.readOctet();

					if ((b1 & QOI_MASK_2) == QOI_INDEX) {
						px = index[b1 ^ QOI_INDEX];
					}
					else if ((b1 & QOI_MASK_3) == QOI_RUN_8) {
						run = (b1 & 0x1f);
					}
					else if ((b1 & QOI_MASK_3) == QOI_RUN_16) {
						int b2 = bs.readOctet();
						run = (((b1 & 0x1f) << 8) | (b2)) + 32;
					}
					else if ((b1 & QOI_MASK_2) == QOI_DIFF_8) {
						px.rgba.r += ((b1 >> 4) & 0x03) - 1;
						px.rgba.g += ((b1 >> 2) & 0x03) - 1;
						px.rgba.b += (b1 & 0x03) - 1;
					}
					else if ((b1 & QOI_MASK_3) == QOI_DIFF_16) {
						int b2 = bs.readOctet();
						px.rgba.r += (b1 & 0x1f) - 15;
						px.rgba.g += (b2 >> 4) - 7;
						px.rgba.b += (b2 & 0x0f) - 7;
					}
					else if ((b1 & QOI_MASK_4) == QOI_DIFF_24) {
						int b2 = bs.readOctet();
						int b3 = bs.readOctet();
						px.rgba.r += (((b1 & 0x0f) << 1) | (b2 >> 7)) - 15;
						px.rgba.g += ((b2 & 0x7c) >> 2) - 15;
						px.rgba.b += (((b2 & 0x03) << 3) | ((b3 & 0xe0) >> 5)) - 15;
						px.rgba.a += (b3 & 0x1f) - 15;
					}
					else if ((b1 & QOI_MASK_4) == QOI_COLOR) {
						if (b1 & 8) { px.rgba.r = bs.readOctet(); }
						if (b1 & 4) { px.rgba.g = bs.readOctet(); }
						if (b1 & 2) { px.rgba.b = bs.readOctet(); }
						if (b1 & 1) { px.rgba.a = bs.readOctet(); }
					}

					index[QOI_COLOR_HASH(px) % 64] = px;
				}

				if (channels == 4) {
					*(qoi_rgba_t*)(pixels + px_pos) = px;
				}
				else {
					pixels[px_pos] = px.rgba.r;
					pixels[px_pos + 1] = px.rgba.g;
					pixels[px_pos + 2] = px.rgba.b;
				}
			}

			return true;
		}

	// Simple interfaces dealing with PixelMap
	// Encode raw RGB or RGBA pixels into a QOI image write it to a stream.
	// w and h denote the the width and height of the pixel data. channels must be 
	// either 3 for RGB data or 4 for RGBA.
	// The function returns false on failure. On success, it will return true 

	static bool write(BinStream& bs, PixelMap& pmap)
	{
		void* data = pmap.getPixelPointer(0, 0);
		int w = pmap.width();
		int h = pmap.height();
		int channels = 4;
		int size;

		bool success = encode(bs, data, w, h, channels, &size);

		return success;
	}

	// Read and decode a QOI image from the stream into a pixelmap 
	// (channels=3) or RGBA (channels=4) pixel data.
	// The function either returns false on failure (invalid data, or malloc or fopen
	// failed) or a pointer to the decoded pixels. 
	// On success out_w and out_h will 
	// be set to the width and height of the decoded image.
	// Read from the BinStream, and decode into the pmap
	// we assume the pmap is already of the right size
	static bool read(BinStream &bs, PixelMap &pmap)
	{
		bool success = decode(bs, pmap);
		return success;
	}
}
















