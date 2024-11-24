#ifndef BITHACKS_H_INCLUDED
#define BITHACKS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

static uint8_t BIT8(size_t bitnum) noexcept {return (uint8_t)1 << bitnum; }
static uint16_t BIT16(size_t bitnum) noexcept {return (uint16_t)1 << bitnum; }
static uint32_t BIT32(size_t bitnum) noexcept {return (uint32_t)1 << bitnum; }
static uint64_t BIT64(size_t bitnum) noexcept {return (uint64_t)1 << bitnum; }

// return true if the specified bit is set in the value
static bool bhak_bit_is_set(const unsigned char value, const size_t bitnum) noexcept {return (value & BIT8(bitnum)) > 0; }

// set a specific bit within a value
static uint64_t bhak_set_bit(const unsigned char value, const size_t bitnum) noexcept {return (value | BIT8(bitnum));}

// unset a specific bit within a value
static int bhak_unset_bit(unsigned char value, size_t bitnum) noexcept { return value & ~BIT8(bitnum);}

//
// getbitbyteoffset()
// 
// Given a bit number, calculate which byte
// it would be in, and which bit within that
// byte.
static void bhak_get_byte_and_bit_offset(size_t bitnumber, size_t *byteoffset, size_t *bitoffset) noexcept
{
    *byteoffset = (int)(bitnumber / 8);
    *bitoffset = bitnumber % 8;
}

// bhak_get_bit_value
// get the value of a single bit in a collection ob bytes
static int bhak_get_bit_value(const uint8_t *bytes, size_t nBytes, size_t bitAt)
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

static int bhak_set_bit_value(uint8_t *bytes, size_t nBytes, size_t bitAt)
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

static int bhak_remove_bit_value(unsigned char *bytes, size_t nBytes, size_t bitAt)
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



#ifdef __cplusplus
}
#endif

#endif      // BITHACKS_H_INCLUDED
