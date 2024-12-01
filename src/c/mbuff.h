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

int mbuff_reset(mbuff *, unsigned char *data, size_t sz) noexcept;
int mbuff_init_from_size(mbuff *, size_t sz) noexcept;
int mbuff_destroy(mbuff*) noexcept;
int mbuff_transfer(mbuff *, mbuff *) noexcept;



// IMPLEMENTATION

int mbuff_reset(mbuff *a, unsigned char *data, size_t sz) noexcept
{
    a->fData = data;
    a->fSize = sz;

    return 0;
}

int mbuff_init_from_size(mbuff *a, size_t sz) noexcept
{
    a->fData = new uint8_t[sz];
    a->fSize = sz;

    return 0;
}

int mbuff_destroy(mbuff *a) noexcept
{
    if (a->fData != nullptr && a->fSize>0)
        delete [] a->fData;

    return 0;
}

int mbuff_transfer(mbuff *a, mbuff *b) noexcept
{
    a->fData = b->fData;
    a->fSize = b->fSize;
    mbuff_reset(b, nullptr, 0);

    return 0;
}

#ifdef __cplusplus
}
#endif

