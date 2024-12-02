#ifndef CONVSPAN_H_INCLUDED
#define CONVSPAN_H_INCLUDED

//
// Convert a bspan to various data types
//
#include "asciiset.h"
#include "bspan.h"

#ifdef __cplusplus
extern "C" {
#endif




static int  hexToDec(const unsigned char vIn, unsigned char *) noexcept;

// First set of routines convert a pointer to a specified numeric type
uint8_t as_u8_le(const unsigned char *a) noexcept;
uint16_t as_u16_le(const unsigned char *a) noexcept;
uint32_t as_u32_le(const unsigned char *a) noexcept;
uint64_t as_u64_le(const unsigned char *a) noexcept;
float as_float_le(const unsigned char *a) noexcept;
double as_double_le(const unsigned char *a) noexcept;

static int bspan_conv_to_u64(const bspan * inChunk, uint64_t * v) noexcept;
static int bspan_conv_to_i64(const bspan * inChunk, uint64_t * v) noexcept;

//
// IMPLEMENTATION
//


static int  hexToDec(const unsigned char vIn, unsigned char *vOut) noexcept
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

uint8_t as_u8_le(const unsigned char *a) noexcept {return (*a);}
uint8_t as_u8_be(const unsigned char *a) noexcept {return (*a);}

uint16_t as_u16_le(const unsigned char *a) noexcept 
{
    return (uint16_t(a[0]) | uint16_t(a[1] << 8));
}

uint16_t as_u16_be(const unsigned char *a) noexcept 
{
    return (uint16_t(a[1]) | uint16_t(a[0] << 8));
}

uint32_t as_u32_le(const unsigned char *a) noexcept
{
    return (uint32_t(a[0]) | uint32_t(a[1] << 8)| uint32_t(a[2] << 16) | uint32_t(a[3] << 24));
}

uint32_t as_u32_be(const unsigned char *a) noexcept
{
    return (uint32_t(a[3]) | uint32_t(a[2] << 8)| uint32_t(a[1] << 16) | uint32_t(a[0] << 24));
}

uint64_t as_u64_le(const unsigned char *a) noexcept
{
    return (uint64_t(a[0]) | uint64_t(a[1]) << 8 | uint64_t(a[2]) << 16 | uint64_t(a[3]) << 24 |
        uint64_t(a[4]) << 32 | uint64_t(a[5]) << 40 | uint64_t(a[6]) << 48 | uint64_t(a[7]) << 56);

}

uint64_t as_u64_be(const unsigned char *a) noexcept
{
    return (uint64_t(a[7]) | uint64_t(a[6]) << 8 | uint64_t(a[5]) << 16 | uint64_t(a[4]) << 24 |
        uint64_t(a[3]) << 32 | uint64_t(a[2]) << 40 | uint64_t(a[1]) << 48 | uint64_t(a[0]) << 56);

}

float as_float_le(const unsigned char *a)
{
    uint32_t i = as_u32_le(a);
    float f;
    memcpy(&f, &i, 4);
    return f;
}

float as_float_be(const unsigned char *a)
{
    uint32_t i = as_u32_be(a);
    float f;
    memcpy(&f, &i, 4);
    return f;
}

double as_double_le(const unsigned char *a)
{
    uint64_t i = as_u64_le(a);
    double d;
    memcpy(&d, &i, 8);
    return d;
}

double as_double_be(const unsigned char *a)
{
    uint64_t i = as_u64_be(a);
    double d;
    memcpy(&d, &i, 8);
    return d;
}

// bspan_conv_to_u64
//
// If successful, the value is stored in the out parameter
//
static int bspan_conv_to_u64(const bspan * inChunk, uint64_t * v) noexcept
{
    bspan s;
    bspan_weak_assign(&s, inChunk);
    
    if (!bspan_is_valid(&s))
        return -1;

    const unsigned char* sStart = bspan_begin(&s);
    const unsigned char* sEnd = bspan_end(&s);

    // Return early if the next thing is not a digit
    if (!is_digit(*sStart))
        return false;

    // Initialize the value
    *v = 0;

    // While we still have input to consume
    while ((sStart < sEnd) && is_digit(*sStart))
    {
        *v = (*v * 10) + (uint64_t)(*sStart - '0');
        sStart++;
    }

    return 0;
}

static int bspan_conv_to_i64(const bspan * inChunk, uint64_t * v) noexcept
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
    int err = bspan_conv_to_u64(&s, &uvalue);
    if (err!=0)
        return err;


        if (sign < 0)
            *v = -(int64_t)uvalue;
        else
            *v = (int64_t)uvalue;

        return true;
}



#ifdef __cplusplus
}
#endif

#endif