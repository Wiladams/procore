#ifndef LEXUTIL_H_INCLUDED
#define LEXUTIL_H_INCLUDED

#include "bspan.h"
#include "charset.h"

#ifdef __cplusplus
extern "C" {
#endif

static bspan lex_ltrim(const bspan* a, const asciiset& skippable) noexcept;
static bspan lex_rtrim(const bspan* a, const asciiset& skippable) noexcept;
static bspan lex_trim(const bspan* a, const asciiset& skippable) noexcept;

#ifdef __cplusplus
}
#endif

#endif      // LEXUTIL_H_INCLUDED