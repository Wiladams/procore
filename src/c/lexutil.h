#ifndef LEXUTIL_H_INCLUDED
#define LEXUTIL_H_INCLUDED

#include "pcoredef.h"
#include "bspan.h"
#include "asciiset.h"

#ifdef __cplusplus
extern "C" {
#endif




static int lex_front_token(const bspan* a, const asciiset *skipleading, const asciiset *delim, bspan *tok, bspan *rest) PC_NOEXCEPT_C;
static int lex_skip_leading_charset(const bspan* a, const asciiset *skippable, bspan *b) PC_NOEXCEPT_C;
static int lex_skip_trailing_charset(const bspan* a, const asciiset *skippable, bspan *b) PC_NOEXCEPT_C;
static int lex_skip_until_charset(const bspan* a, const asciiset *skippable, bspan *b, bspan *) PC_NOEXCEPT_C;

static int lex_ltrim(bspan* a, const asciiset * skippable) PC_NOEXCEPT_C;
static int lex_rtrim(bspan* a, const asciiset * skippable) PC_NOEXCEPT_C;
static int lex_trim(bspan* a, const asciiset * skippable) PC_NOEXCEPT_C;

static bool lex_begins_with_span(const bspan * a, const bspan * b) PC_NOEXCEPT_C;
static int lex_read_quoted(const bspan *src, bspan *dataChunk, bspan *rest) PC_NOEXCEPT_C;


// Implementation
// lex_skip_leading_charset
// Skip any characters if they are on the front of the span
static int lex_skip_leading_charset(const bspan* a, const asciiset *skippable, bspan *b) PC_NOEXCEPT_C
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

static int lex_skip_trailing_charset(const bspan* a, const asciiset *skippable, bspan *b) PC_NOEXCEPT_C
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

static int lex_ltrim(bspan* a, const asciiset * skippable) PC_NOEXCEPT_C
{
    return lex_skip_leading_charset(a, skippable, a);
}

static int lex_rtrim(bspan* a, const asciiset * skippable) PC_NOEXCEPT_C
{
    return lex_skip_trailing_charset(a, skippable, a);
}

static int lex_trim(bspan* a, const asciiset * skippable) PC_NOEXCEPT_C
{
    int err = lex_ltrim(a, skippable);
    if (err != 0)
        return err;

    err = lex_rtrim(a, skippable);
    
    return err;
}



static int lex_skip_until_charset(const bspan* a, const asciiset *skippable, bspan *tok, bspan *rest) PC_NOEXCEPT_C
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

static int lex_front_token(const bspan* a, const asciiset *skipleading, const asciiset *delim, bspan *tok, bspan *rest) PC_NOEXCEPT_C
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
static bool lex_begins_with_span(const bspan * a, const bspan * b) PC_NOEXCEPT_C
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

	// chunk_find_char
	// Given an input chunk
	// find the first instance of a specified character
	// return the chunk preceding the found character
	// or or the whole chunk of the character is not found
	//
	static int lex_find_char(const bspan * a, unsigned char c, bspan *rest) PC_NOEXCEPT_C
	{
		const unsigned char * start = bspan_begin(a);
		const unsigned char * end = bspan_end(a);
		while (start < end && *start != c)
			++start;

        bspan_init_from_pointers(rest, start, end);

        return 0;

	}

// lex_find_cstr
// Find the location of a c-string within the source span
// Return
//   0 - no error
// rest - begin will be where the search string was found
//        end will be the end of the search string that was found

static int lex_find_cstr(const bspan* a, const char* cstr, bspan *rest) PC_NOEXCEPT_C
{
		const uint8_t* start = bspan_begin(a);
		const uint8_t* end = bspan_end(a);
		const uint8_t* cstart = (const unsigned char*)cstr;
		const uint8_t* cend = cstart + strlen(cstr);

        bspan cspan;
        bspan_init_from_pointers(&cspan, cstart, cend);

        bspan sspan;

        while (start < end)
        {
            bspan_init_from_pointers(&sspan, start, end);
            if (!lex_begins_with_span(&sspan, &cspan))
                ++start;
        }

        bspan_init_from_pointers(rest, start, end);

        return 0;

}

//
// lex_read_quoted()
// 
// Read a quoted string from the input stream
// Read a first quote, then use that as the delimiter
// to read to the end of the string
//
// the 'dataChunk' will contain the content of what was between
// the quote characters.
//
// the 'rest' param will contain the remainder of the src
//
static int lex_read_quoted(const bspan *src, bspan *dataChunk, bspan *rest) PC_NOEXCEPT_C
{
    const unsigned char* beginValue = nullptr;
    const unsigned char* endValue = nullptr;
    unsigned char quote{};

    
    if (!bspan_is_valid(src))
        return -1;

    bspan_weak_assign(rest, src);

    // capture the quote character
    quote = bspan_front(rest);

    // advance past the quote, then look for the matching close quote
    bspan_advance(rest, 1);
    beginValue = bspan_begin(rest);

    // Skip until end of the value.
    while (bspan_is_valid(rest) && bspan_front(rest) != quote)
        bspan_advance(rest,1);

    if (bspan_is_valid(rest))
    {
        endValue = bspan_begin(rest);
        bspan_advance(rest, 1);
    }

    // Store only well formed attributes
    bspan_init_from_pointers(dataChunk, beginValue, endValue);

    return 0;
}

// lex_read_bracketed
//
// This is a slightly different semantic than lex_read_quoted
// In this case, we know the 'opening' and 'closing' that we're looking for, whereas
// in the previous one, we don't know what the opening 'quote' character is.
// Also, in this case, the opening and closing can be different, like '[' ']'
//
// Given a chunk, read the contents of the chunk until the matching close bracket is found
// The src span is advanced past the closing bracket
//
static int lex_read_bracketed(bspan * src, const unsigned char lbracket, const unsigned char rbracket, bspan *dataChunk) PC_NOEXCEPT_C
{
	const unsigned char * beginattrValue = nullptr;
	const unsigned char * endattrValue = nullptr;
	uint8_t quote{};

	// Skip white space before the quoted bytes
	lex_ltrim(src, wspcharset());

	if (!bspan_is_valid(src) || bspan_front(src) != lbracket)
		return -1;


	// advance past the lbracket, then look for the matching close quote
	bspan_advance(src, 1);
	beginattrValue = bspan_begin(src);


	// Skip until end of the value.
	while (bspan_is_valid(src) && bspan_front(src) != rbracket)
		bspan_advance(src, 1);

	if (bspan_is_valid(src))
	{
		endattrValue = bspan_begin(src);
		bspan_advance(src,1);
	}

	// Store only well formed quotes
    bspan_init_from_pointers(dataChunk, beginattrValue, endattrValue);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif      // LEXUTIL_H_INCLUDED