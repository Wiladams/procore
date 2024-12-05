#pragma once

#include "bspan.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mbuff_t {
    unsigned char *fData;
    size_t fSize;
};
typedef struct mbuff_t mbuff;

int mbuff_reset(mbuff *, unsigned char *data, size_t sz) PC_NOEXCEPT_C;
int mbuff_init_from_size(mbuff *, size_t sz) PC_NOEXCEPT_C;
int mbuff_destroy(mbuff*) PC_NOEXCEPT_C;
int mbuff_transfer(mbuff *, mbuff *) PC_NOEXCEPT_C;



// IMPLEMENTATION

int mbuff_reset(mbuff *a, unsigned char *data, size_t sz) PC_NOEXCEPT_C
{
    a->fData = data;
    a->fSize = sz;

    return 0;
}

int mbuff_init_from_size(mbuff *a, size_t sz) PC_NOEXCEPT_C
{
    a->fData = new uint8_t[sz];
    a->fSize = sz;

    return 0;
}

int mbuff_destroy(mbuff *a) PC_NOEXCEPT_C
{
    if (a->fData != nullptr && a->fSize>0)
        delete [] a->fData;

    return 0;
}

int mbuff_transfer(mbuff *a, mbuff *b) PC_NOEXCEPT_C
{
    a->fData = b->fData;
    a->fSize = b->fSize;
    mbuff_reset(b, nullptr, 0);

    return 0;
}

#ifdef __cplusplus
}
#endif

