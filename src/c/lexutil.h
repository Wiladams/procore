#ifndef LEXUTIL_H_INCLUDED
#define LEXUTIL_H_INCLUDED

#include "bspan.h"
#include "asciiset.h"

#ifdef __cplusplus
extern "C" {
#endif

static int lex_front_token(const bspan* a, const asciiset *skipleading, const asciiset *delim, bspan *tok, bspan *rest) noexcept;
static int lex_skip_leading_charset(const bspan* a, const asciiset *skippable, bspan *b) noexcept;
static int lex_skip_trailing_charset(const bspan* a, const asciiset *skippable, bspan *b) noexcept;
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

static int lex_skip_trailing_charset(const bspan* a, const asciiset *skippable, bspan *b) noexcept
{
    if (!bspan_is_valid(a))
        return -1;

    bspan_weak_assign(b, a);
    unsigned char *endAt = (unsigned char *)b->fEnd-1;
    while (endAt > b->fStart){
        if (asciiset_contains_char(skippable, *endAt))
            endAt--;
        else
            break;
    }
    if (endAt != b->fStart)
        b->fEnd = endAt;

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

static int lex_front_token(const bspan* a, const asciiset *skipleading, const asciiset *delim, bspan *tok, bspan *rest) noexcept
{
    bspan_weak_assign(rest, a);
    
    // skip any skippable front characters, typically whitespace
    lex_skip_leading_charset(rest, skipleading, rest);

    // start the token off matching the rest of the span
    // if there's no delimeters, then the token is the remainder
    bspan_weak_assign(tok, rest);

    // Now figure out where the token delimeter is
    unsigned char *endAt = (unsigned char *)tok->fStart;
    while (endAt < tok->fEnd){
        if (asciiset_contains_char(delim, *endAt))
            break;
        else
            endAt++;
    }
    
    // If we ran into a delimeter character, then endAt
    // will not equal the end, so mark the token end
    // And mark the rest beginning to be just past that
    if (endAt != tok->fEnd)
    {
        tok->fEnd = endAt;
        endAt++;
        rest->fStart = endAt;
    } else {
        // if the delimeter was not found, then make the 'rest'
        // be blank, while the token is the remainder
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