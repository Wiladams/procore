#ifndef BITHACKS_H_INCLUDED
#define BITHACKS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "pcoredef.h"

static INLINE bool bhak_isLE() PC_NOEXCEPT_C { int i = 1; return (int)*((unsigned char*)&i) == 1; }
static INLINE bool bhak_isBE() PC_NOEXCEPT_C { return !bhak_isLE(); }

static uint8_t BIT8(size_t bitnum) PC_NOEXCEPT_C {return (uint8_t)1 << bitnum; }
static uint16_t BIT16(size_t bitnum) PC_NOEXCEPT_C {return (uint16_t)1 << bitnum; }
static uint32_t BIT32(size_t bitnum) PC_NOEXCEPT_C {return (uint32_t)1 << bitnum; }
static uint64_t BIT64(size_t bitnum) PC_NOEXCEPT_C {return (uint64_t)1 << bitnum; }

// return true if the specified bit is set in the value
static bool bhak_bit_is_set(const unsigned char value, const size_t bitnum) PC_NOEXCEPT_C {return (value & BIT8(bitnum)) > 0; }

// set a specific bit within a value
static uint64_t bhak_set_bit(const unsigned char value, const size_t bitnum) PC_NOEXCEPT_C {return (value | BIT8(bitnum));}

// unset a specific bit within a value
static int bhak_unset_bit(unsigned char value, size_t bitnum) PC_NOEXCEPT_C { return value & ~BIT8(bitnum);}

//
// getbitbyteoffset()
// 
// Given a bit number, calculate which byte
// it would be in, and which bit within that
// byte.
static void bhak_get_byte_and_bit_offset(size_t bitnumber, size_t *byteoffset, size_t *bitoffset) PC_NOEXCEPT_C
{
    *byteoffset = (int)(bitnumber / 8);
    *bitoffset = bitnumber % 8;
}

// bhak_get_bit_value
// get the value of a single bit in a collection of bytes
// -1   error, out of range
//  1   bit is set to 1
//  0   bit is set to 0
//
static int bhak_get_bit_value(const uint8_t *bytes, size_t nBytes, size_t bitAt) PC_NOEXCEPT_C
{
    size_t byteoffset=0;
    size_t bitoffset=0;
    bhak_get_byte_and_bit_offset(bitAt, &byteoffset, &bitoffset);

        // index out of bounds
    if (byteoffset >= nBytes)
        return -1;

    bool bitval = bhak_bit_is_set(bytes[byteoffset], bitoffset);

    return bitval;
}

static int bhak_set_bit_value(uint8_t *bytes, size_t nBytes, size_t bitAt) PC_NOEXCEPT_C
{
    size_t byteoffset=0;
    size_t bitoffset=0;
    bhak_get_byte_and_bit_offset(bitAt, &byteoffset, &bitoffset);

    // index out of bounds
    if (byteoffset >= nBytes)
        return -1;

    unsigned char newValue = bhak_set_bit(bytes[byteoffset], bitoffset);
    bytes[byteoffset] = newValue;

    return 0;
}

static int bhak_remove_bit_value(unsigned char *bytes, size_t nBytes, size_t bitAt) PC_NOEXCEPT_C
{
    size_t byteoffset=0;
    size_t bitoffset=0;
    bhak_get_byte_and_bit_offset(bitAt, &byteoffset, &bitoffset);

    // index out of bounds
    if (byteoffset >= nBytes)
        return -1;

    unsigned char newValue = bhak_unset_bit(bytes[byteoffset], bitoffset);
    bytes[byteoffset] = newValue;

    return 0;
}

// bhak_sign_extend_u16
// Take a smaller negative number, and sign extend
// to 16 bits
static uint16_t bhak_sign_extend_u16(uint16_t x, int bit_count) PC_NOEXCEPT_C
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

static int64_t bhak_sign_extend_u64(uint64_t val, int bit_count) PC_NOEXCEPT_C
{
    return ((int64_t)(val << (64 - bit_count))) >> (64 - bit_count);
}

#ifdef __cplusplus
}
#endif

#endif      // BITHACKS_H_INCLUDED
