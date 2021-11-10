#pragma once

/*
    Convenient routines for fiddling with bits in integers.
    Primarily setting and getting bits
    or swapping bytes in an integer.
*/

#include <math.h>
#include <cstdio>

#define U64x(hi, lo)	(((uint64_t)0x##hi << 32) + (uint64_t)0x##lo)


namespace winme {
// Determine at runtime if the CPU is little-endian (intel standard)
static inline bool isLE () {
    int i=1;
    return (int)*((unsigned char *)&i)==1;
}

// return at runtime whether machine is Big-Endian
static inline bool isBE() {return !isLE();}



// Some things for 32-bit integers
// there may be intrinsics for these, but 
// the following are portable implementations
const int i4_huge = 2147483647;
static inline bool i4_is_even(int i) { int value = ((i % 2) == 0); return value; }
static inline bool i4_is_odd(int i) { int value = ((i % 2) != 0); return value; }
static inline bool i4_is_power_of_two(int n) {
    if (n <= 0)
    {
        return false;
    }

    while (1 < n)
    {
        if ((n % 2) == 1) {
            return false;
        }
        n = n / 2;
    }

    return true;
}
static inline int i4_max(int i1, int i2) { return i2 < i1 ? i1 : i2; }
static inline int i4_min(int i1, int i2) { return i2 > i1 ? i1 : i2; }

// Returns the 2-way sign of an integer
// very important that 0 is 'positive' along with '>0'
//
// I < 0 i4_sign(I) == -1
// I >= 0   i4_sign(I) == 1
//
static inline int i4_sign(int i) {
    if (i<0)
        return -1;

    return 1;
}

// Returns the 3-way sign of an integer
//
// I < 0    sign(I) = -1
// I == 0   sign(I) = 0
// I > 0    sign(I) = 1
//
static inline int i4_sign3(int val){ return ((0 < val) - (val < 0)); }


// Return various forms of pow(2,bitnum)
// There are different ones, which allow the user to specify how
// many bits they want
static inline uint8_t BIT8(size_t bitnum) {return (uint8_t)1 << bitnum; }
static inline uint16_t BIT16(size_t bitnum) {return (uint16_t)1 << bitnum; }
static inline uint32_t BIT32(size_t bitnum) {return (uint32_t)1 << bitnum; }
static inline uint64_t BIT64(size_t bitnum) {return (uint64_t)1 << bitnum; }

// One general purpose which will default to BIT64
//static inline uint64_t BIT(unsigned int bitnum) {return BIT64(bitnum);}

// return true if the specified bit is set in the value
static inline bool isset(const uint64_t value, const size_t bitnum) {return (value & BIT64(bitnum)) > 0; }

// set a specific bit within a value
static inline uint64_t i8_set_bit(const uint64_t value, const size_t bitnum) { return (value | BIT64(bitnum)); }
//static inline uint64_t setbit(const uint64_t value, const size_t bitnum) {return (value | BIT64(bitnum));}

// BITMASK64
// A bitmask is an integer where all the bits from the 
// specified low value to the specified high value
// are set to 1.
// The trick used in BITMASK64 is to set a single bit
// above the required range, then subtracting 1 from that
// value.  By subtracting 1, we get all 1 bits below the
// single bit value.
// This set of 1 bits are then shifted upward by the number
// of the low bit, and we have our mask.
// Discussion: 
//   https://stackoverflow.com/questions/28035794/masking-bits-within-a-range-given-in-parameter-in-c
//
//  uint64_t mask = (uint64_t)1 << (high-low);
//  mask <<= 1;
//  mask--;         // turn on all the bits below
//  mask <<= low;   // shift up to proper position
//  return mask;

static inline uint64_t BITMASK64(const size_t low, const size_t high)
{
    return ((((uint64_t)1 << (high-low)) << 1) - 1) << low;
}

static inline uint8_t BITMASK8(const size_t low, const size_t high) {return (uint8_t)BITMASK64(low, high);}
static inline uint16_t BITMASK16(const size_t low, const size_t high) {return (uint16_t)BITMASK64(low,high);}
static inline uint32_t BITMASK32(const size_t low, const size_t high) {return (uint32_t)BITMASK64(low, high);}

//#define BITMASK BITMASK64


// BITSVALUE
// Retrieve a value from a lowbit highbit pair
// This is useful when you're trying to unpack bits that 
// are in an integer
static inline  uint64_t BITSVALUE(uint64_t src, size_t lowbit, size_t highbit)
{
    return ((src & BITMASK64(lowbit, highbit)) >> lowbit);
}

// Given a bit number, calculate which byte
// it would be in, and which bit within that
// byte.
static inline void getbitbyteoffset(size_t bitnumber, size_t &byteoffset, size_t &bitoffset)
{
    byteoffset = (int)(bitnumber / 8);
    bitoffset = bitnumber % 8;
}

//
// Return an integer value from an array of bytes
static inline uint64_t bitsValueFromBytes(const uint8_t *bytes, const size_t startbit, const size_t bitcount, bool bigendian = false)
{
    // Sanity check
    if (nullptr == bytes)
        return 0;

    uint64_t value = 0;

    if (bigendian) {
        for (int i=bitcount; i>= 0; i--) {
            size_t byteoffset=0;
            size_t bitoffset=0;
            getbitbyteoffset(startbit+i, byteoffset, bitoffset);
            bool bitval = isset(bytes[byteoffset], bitoffset);
//printf("byte, bit: %Id, %Id, %d\n", byteoffset, bitoffset, bitval);
		    if (bitval) {
			    value = i8_set_bit(value, i);
            }
        }
    } else {
        for (size_t i=0; i<bitcount; i++) {
            size_t byteoffset=0;
            size_t bitoffset=0;
            getbitbyteoffset(startbit+i, byteoffset, bitoffset);
            bool bitval = isset(bytes[byteoffset], bitoffset);
//printf("byte, bit: %Id, %Id, %d\n", byteoffset, bitoffset, bitval);
            if (bitval) {
                value = i8_set_bit(value, i);
            }
        }
    }

    return value;
}



// swap 2 bytes (16-bit) around
static inline uint16_t swapUInt16(const uint16_t num)
{
    return (((num & 0x00ff) << 8) | ((num & 0xff00) >> 8));
}

// swap 4 bytes (32-bit) around
static inline uint32_t swapUInt32(const uint32_t num)
{
    uint32_t x = (num & 0x0000FFFF) << 16 | (num & 0xFFFF0000) >> 16;
    x = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;

    return x;
}

// swap 8 bytes (64-bit) around
static inline uint64_t swapUInt64(const uint64_t num)
{
    return  (num >> 56) |
          ((num<<40) & 0x00FF000000000000) |
          ((num<<24) & 0x0000FF0000000000) |
          ((num<<8) & 0x000000FF00000000) |
          ((num>>8) & 0x00000000FF000000) |
          ((num>>24) & 0x0000000000FF0000) |
          ((num>>40) & 0x000000000000FF00) |
          (num << 56);
}

// Typically used in a graphics context where we want to allocated byte
// aligned chunks of memory based on a good integer size.
// when bitsperpixel/8 == alignment, then things are very simple
// otherwise, there's some padding.
static inline int GetAlignedByteCount(const int width, const int bitsperpixel, const int alignment)
{
    return (((width * (bitsperpixel / 8)) + (alignment - 1)) & ~(alignment - 1));
}
}; // namespace ndt