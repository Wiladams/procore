#pragma once

//
// A mbuff is a simple buffer that holds a pointer to a block of memory and its size.
// the mbuff owns the data and if you call destroy() it will be freed.
// This looks very similar to a bspan, but is different in terms of ownership of 
// the data.  Also, the bspan is often used as a 'cursor' on data, and many functions
// are provided for parsing and otherwise manipulating spans.
// The mbuff is more basic, and a bspan can be formed on top of an mbuf

#include "pcoredef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mbuff_t {
    unsigned char *fData;
    size_t fSize;
};
typedef struct mbuff_t mbuff;

static int mbuff_init() PC_NOEXCEPT_C;
static int mbuff_create_from_size(mbuff *, size_t sz) PC_NOEXCEPT_C;
static int mbuff_destroy(mbuff*) PC_NOEXCEPT_C;
static int mbuff_transfer(mbuff *, mbuff *) PC_NOEXCEPT_C;

static size_t mbuff_size(const mbuff *a) PC_NOEXCEPT_C;
static unsigned char *mbuff_data(const mbuff *a) PC_NOEXCEPT_C;
static unsigned char *mbuff_begin(const mbuff *a) PC_NOEXCEPT_C;
static unsigned char *mbuff_end(const mbuff *a) PC_NOEXCEPT_C;



// IMPLEMENTATION

static int mbuff_init(mbuff *a) PC_NOEXCEPT_C
{
    a->fData = nullptr;
    a->fSize = 0;

    return 0;
}

static int mbuff_create_from_size(mbuff *a, size_t sz) PC_NOEXCEPT_C
{
    a->fData = new uint8_t[sz];
    a->fSize = sz;

    return 0;
}


static int mbuff_destroy(mbuff *a) PC_NOEXCEPT_C
{
    if (a->fData != nullptr && a->fSize>0)
        delete [] a->fData;
    a->fData = nullptr;
    a->fSize = 0;

    return 0;
}

// Transfer ownership of data from b to a
// b will be reset to nullptr
static int mbuff_transfer(mbuff *a, mbuff *b) PC_NOEXCEPT_C
{
    a->fData = b->fData;
    a->fSize = b->fSize;
    mbuff_reset(b, nullptr, 0);

    return 0;
}

static size_t mbuff_size(const mbuff *a) PC_NOEXCEPT_C { return a->fSize; }
static unsigned char *mbuff_data(const mbuff *a) PC_NOEXCEPT_C { return a->fData; }
static unsigned char *mbuff_begin(const mbuff *a) PC_NOEXCEPT_C { return a->fData; }
static unsigned char *mbuff_end(const mbuff *a) PC_NOEXCEPT_C { return a->fData + a->fSize; }



#ifdef __cplusplus
}
#endif

