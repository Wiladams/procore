#ifndef CONVSPAN_H_INCLUDED
#define CONVSPAN_H_INCLUDED

#include <cmath>

#include "pcoredef.h"
#include "asciiset.h"
#include "bspan.h"

//
// Convert a bspan to various data types
// As welll as base numeric type conversions from pointers
//

#ifdef __cplusplus
extern "C" {
#endif





// First set of routines convert a pointer to a specified numeric type
static uint8_t as_u8_le(const unsigned char *a) PC_NOEXCEPT;
static uint16_t as_u16_le(const unsigned char *a) PC_NOEXCEPT;
static uint32_t as_u32_le(const unsigned char *a) PC_NOEXCEPT;
static uint64_t as_u64_le(const unsigned char *a) PC_NOEXCEPT;
static float as_float_le(const unsigned char *a) PC_NOEXCEPT;
static double as_double_le(const unsigned char *a) PC_NOEXCEPT;

static int  hexToDec(const unsigned char vIn, unsigned char *) PC_NOEXCEPT;

static int bspan_conv_hex_to_u64(const bspan * inSpan, uint64_t *outValue, bspan *rest) PC_NOEXCEPT;
static int bspan_conv_to_u64(const bspan * inChunk, uint64_t * v, bspan *rest) PC_NOEXCEPT;
static int bspan_conv_to_i64(const bspan * inChunk, uint64_t * v, bspan *rest) PC_NOEXCEPT;
static int bspan_conv_to_double(const bspan * inChunk, double * v, bspan *rest) PC_NOEXCEPT;

//
// IMPLEMENTATION
//




static uint8_t as_u8_le(const unsigned char *a) PC_NOEXCEPT {return (*a);}
static uint8_t as_u8_be(const unsigned char *a) PC_NOEXCEPT {return (*a);}

static uint16_t as_u16_le(const unsigned char *a) PC_NOEXCEPT 
{
    return (uint16_t(a[0]) | uint16_t(a[1] << 8));
}

static uint16_t as_u16_be(const unsigned char *a) PC_NOEXCEPT 
{
    return (uint16_t(a[1]) | uint16_t(a[0] << 8));
}

static uint32_t as_u32_le(const unsigned char *a) PC_NOEXCEPT
{
    return (uint32_t(a[0]) | uint32_t(a[1] << 8)| uint32_t(a[2] << 16) | uint32_t(a[3] << 24));
}

static uint32_t as_u32_be(const unsigned char *a) PC_NOEXCEPT
{
    return (uint32_t(a[3]) | uint32_t(a[2] << 8)| uint32_t(a[1] << 16) | uint32_t(a[0] << 24));
}

static uint64_t as_u64_le(const unsigned char *a) PC_NOEXCEPT
{
    return (uint64_t(a[0]) | uint64_t(a[1]) << 8 | uint64_t(a[2]) << 16 | uint64_t(a[3]) << 24 |
        uint64_t(a[4]) << 32 | uint64_t(a[5]) << 40 | uint64_t(a[6]) << 48 | uint64_t(a[7]) << 56);

}

static uint64_t as_u64_be(const unsigned char *a) PC_NOEXCEPT
{
    return (uint64_t(a[7]) | uint64_t(a[6]) << 8 | uint64_t(a[5]) << 16 | uint64_t(a[4]) << 24 |
        uint64_t(a[3]) << 32 | uint64_t(a[2]) << 40 | uint64_t(a[1]) << 48 | uint64_t(a[0]) << 56);

}

static float as_float_le(const unsigned char *a) PC_NOEXCEPT
{
    uint32_t i = as_u32_le(a);
    float f;
    memcpy(&f, &i, 4);
    return f;
}

static float as_float_be(const unsigned char *a) PC_NOEXCEPT
{
    uint32_t i = as_u32_be(a);
    float f;
    memcpy(&f, &i, 4);
    return f;
}

static double as_double_le(const unsigned char *a) PC_NOEXCEPT
{
    uint64_t i = as_u64_le(a);
    double d;
    memcpy(&d, &i, 8);
    return d;
}

static double as_double_be(const unsigned char *a) PC_NOEXCEPT
{
    uint64_t i = as_u64_be(a);
    double d;
    memcpy(&d, &i, 8);
    return d;
}

static int  hexToDec(const unsigned char vIn, unsigned char *vOut) PC_NOEXCEPT
{
    if (vIn >= '0' && vIn <= '9')
        *vOut = vIn - '0';
    else if (vIn >= 'a' && vIn <= 'f')
        *vOut = vIn - 'a' + 10;
    else if (vIn >= 'A' && vIn <= 'F')
        *vOut = vIn - 'A' + 10;
    else
        return -1;

    return 0;
}

// bspan_conv_hex_to_u64
//
// Specified as:  0xffeeddffaaccee55
// The leading '0x' is not specified
//
static  int bspan_conv_hex_to_u64(const bspan * inSpan, uint64_t *outValue, bspan *rest) PC_NOEXCEPT
{
    if (!bspan_is_valid(inSpan))
        return -1;

    size_t sz = bspan_size(inSpan);


    // building up outValue as we go
    *outValue = 0;
    const unsigned char * sStart = bspan_begin(inSpan);
    const unsigned char * sEnd = bspan_end(inSpan);

	while (sStart < sEnd)
	{
        unsigned char vOut=0;
        if (hexToDec(*sStart, &vOut) !=0)
            return -1;

        // We shift by 4 bits, because we're processing a nibble at a time
        // and a nibble is 4 bits
		*outValue <<= 4;
		*outValue |= vOut;

        sStart++;
	}
        
    if (rest!=nullptr){
        bspan_init_from_pointers(rest, sStart, sEnd);
    }

    return 0;
}

// bspan_conv_to_u64
//
// If successful, the value is stored in the out parameter
//
static int bspan_conv_to_u64(const bspan * inChunk, uint64_t * v, bspan *rest) PC_NOEXCEPT
{
    bspan s;
    bspan_weak_assign(&s, inChunk);
    
    if (!bspan_is_valid(&s))
        return -1;

    const unsigned char* sStart = bspan_begin(&s);
    const unsigned char* sEnd = bspan_end(&s);

    // Return early if the next thing is not a digit
    if (!is_digit(*sStart))
        return -1;

    // Initialize the value
    *v = 0;

    // While we still have input to consume
    while ((sStart < sEnd) && is_digit(*sStart))
    {
        *v = (*v * 10) + (uint64_t)(*sStart - '0');
        sStart++;
    }

    if (rest != nullptr){
        bspan_init_from_pointers(rest, sStart, s.fEnd);
    }

    return 0;
}

static int bspan_conv_to_i64(const bspan * inChunk, uint64_t * v, bspan *rest) PC_NOEXCEPT
{
    if (!bspan_is_valid(inChunk))
        return -1;
    
    bspan s;
    bspan_weak_assign(&s, inChunk);

    const unsigned char* sStart = s.fStart;
    const unsigned char* sEnd = s.fEnd;

    // Check for a sign if it's there
    int sign = 1;
    if (*sStart == '-') {
        sign = -1;
        sStart++;
    }
    else if (*sStart == '+') {
        sStart++;
    }

    uint64_t uvalue{ 0 };
    s.fStart = sStart;
    int err = bspan_conv_to_u64(&s, &uvalue, rest);
    if (err!=0)
        return err;


    if (sign < 0)
            *v = -(int64_t)uvalue;
    else
            *v = (int64_t)uvalue;

    return true;
}

    // Parse a double number from the given ByteSpan, advancing the start
    // of the ByteSpan to beyond where we found the last character of the number.
    // Assumption:  We're sitting at beginning of a number, all whitespace handling
    // has already occured.
static int bspan_conv_to_double(const bspan * inChunk, double * v, bspan *rest) PC_NOEXCEPT
{
    if (!bspan_is_valid(inChunk))
        return -1;

    bspan s;
    bspan_weak_assign(&s, inChunk);

    const unsigned char* startAt = bspan_begin(&s);
    const unsigned char* endAt = bspan_end(&s);

    double sign = 1.0;
    double res = 0.0;

    // integer part
    uint64_t intPart = 0;

    // fractional part
    uint64_t fracPart = 0;
    uint64_t fracBase = 1;


    bool hasIntPart = false;
    bool hasFracPart = false;
    bspan remains;

    // Parse optional sign
    if (*startAt == '+') {
        startAt++;
    }
    else if (*startAt == '-') {
        sign = -1;
        startAt++;
    }

    // Parse integer part
    if (is_digit(*startAt))
    {
        hasIntPart = true;
        s.fStart = startAt;
        bspan_conv_to_u64(&s, &intPart, &remains);

        startAt = remains.fStart;
        res = static_cast<double>(intPart);
    }

    // Parse fractional part.
    if ((startAt < endAt) && (*startAt == '.'))
    {
        hasFracPart = true;
        startAt++; // Skip '.'

        fracBase = 1;

        // Add the fraction portion without calling out to powd
        while ((startAt < endAt) && is_digit(*startAt)) {
            fracPart = fracPart * 10 + static_cast<uint64_t>(*startAt - '0');
            fracBase *= 10;
            startAt++;
        }
        res += (static_cast<double>(fracPart) / static_cast<double>(fracBase));

    }

    // If we don't have an integer or fractional
    // part, then just return false
    if (!hasIntPart && !hasFracPart)
        return -1;

    // Parse optional exponent
    // mostly we don't see this, so we won't bother trying
    // to optimize it beyond using powd
    if ((startAt < endAt) && 
        (((*startAt == 'e') || (*startAt == 'E')) && 
        ((startAt[1] != 'm') && (startAt[1] != 'x'))))
    {
        // exponent parts
        uint64_t expPart = 0;
        double expSign = 1.0;

        startAt++; // skip 'E'
        if (*startAt == '+') {
            startAt++;
        }
        else if (*startAt == '-') {
            expSign = -1.0;
            startAt++;
        }


        if (is_digit(*startAt)) {
            s.fStart = startAt;
            bspan_conv_to_u64(&s, &expPart, &remains);
            startAt = bspan_begin(&remains);
            res = res * std::pow(10, double(expSign * double(expPart)));
        }
    }
    s.fStart = startAt;

    *v = res * sign;

    return 0;
}


#ifdef __cplusplus
}
#endif

#endif