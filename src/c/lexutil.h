#ifndef LEXUTIL_H_INCLUDED
#define LEXUTIL_H_INCLUDED

#include "bspan.h"
#include "asciiset.h"

#ifdef __cplusplus
extern "C" {
#endif

static int lex_skip_leading_charset(const bspan* a, const asciiset *skippable, bspan *b) noexcept;
static int lex_skip_until_charset(const bspan* a, const asciiset *skippable, bspan *b, bspan *) noexcept;

static int lex_ltrim(const bspan* a, const asciiset * skippable, bspan *b) noexcept;
static int lex_rtrim(const bspan* a, const asciiset * skippable, bspan *b) noexcept;
static int lex_trim(const bspan* a, const asciiset * skippable, bspan *b) noexcept;

static bool lex_begins_with_span(const bspan * a, const bspan * b) noexcept;


// Implementation
// lex_skip_leading_charset
// Skip any characters if they are on the front of the span
static int lex_skip_leading_charset(const bspan* a, const asciiset *skippable, bspan *b) noexcept
{
    bspan_weak_assign(b, a);
    unsigned char *startAt = (unsigned char *)b->fStart;
    while (startAt < b->fEnd){
        if (asciiset_contains_char(skippable, *startAt))
            startAt++;
        else
            break;
    }
    if (startAt != b->fEnd)
        b->fStart = startAt;

    return 0;
}

static int lex_skip_until_charset(const bspan* a, const asciiset *skippable, bspan *tok, bspan *rest) noexcept
{
    bspan_weak_assign(tok, a);
    bspan_weak_assign(rest, a);

    unsigned char *endAt = (unsigned char *)tok->fStart;
    while (endAt < tok->fEnd){
        if (asciiset_contains_char(skippable, *endAt))
            break;
        else
            endAt++;
    }
    
    if (endAt != tok->fEnd)
    {
     tok->fEnd = endAt;
     endAt++;
     rest->fStart = endAt;
    } else {
        rest->fStart = rest->fEnd;
    }

    return 0;
}


	// bspan_begins_with_bspan()
	// This is the first one with the cleanest semantics
	// It is a byte comparison, assuming the first operand is at 
	// least as big as the second, and that every byte from the second
	// operand is located in sequence in the first operand.
	static bool lex_begins_with_span(const bspan * a, const bspan * b) noexcept
	{
		size_t len = bspan_size(b);
		
		if (len > bspan_size(a))
			return false;

		const unsigned char *cs = bspan_begin(a);
		const unsigned char *ct = bspan_begin(b);

		for (int i=0;i<len;i++,cs++, ct++)
  		{
			if (*cs != *ct)
				return false;
  		}
  		return true;
	}
#ifdef __cplusplus
}
#endif

#endif      // LEXUTIL_H_INCLUDED