#pragma once

#include "pconfig.hpp"

//	Herein you will find various objects and functions which are
//	useful while scanning, tokenizing, parsing streams of text.

#include "bspan.h"
#include "charset.h"


namespace pcore
{
		static inline size_t copy_to_cstr(char* str, size_t len, const ByteSpan& a) noexcept;
		static inline ByteSpan chunk_ltrim(const ByteSpan& a, const charset& skippable) noexcept;
		static inline ByteSpan chunk_rtrim(const ByteSpan& a, const charset& skippable) noexcept;
		static inline ByteSpan chunk_trim(const ByteSpan& a, const charset& skippable) noexcept;
		static inline ByteSpan chunk_skip_wsp(const ByteSpan& a) noexcept;
		
		static inline ByteSpan chunk_subchunk(const ByteSpan& a, const size_t start, const size_t sz) noexcept;
		static inline ByteSpan chunk_take(const ByteSpan& dc, size_t n) noexcept;

		static inline bool chunk_starts_with(const ByteSpan& a, const ByteSpan& b) noexcept;
		static inline bool chunk_starts_with_char(const ByteSpan& a, const uint8_t b) noexcept;
		static inline bool chunk_starts_with_cstr(const ByteSpan& a, const char* b) noexcept;
		
		static inline bool chunk_ends_with(const ByteSpan& a, const ByteSpan& b) noexcept;
		static inline bool chunk_ends_with_char(const ByteSpan& a, const uint8_t b) noexcept;
		static inline bool chunk_ends_with_cstr(const ByteSpan& a, const char* b) noexcept;
		
		static inline ByteSpan chunk_token(ByteSpan& a, const charset& delims) noexcept;
		static inline ByteSpan chunk_find_char(const ByteSpan& a, char c) noexcept;

		// Number Conversions
		static inline double chunk_to_double(const ByteSpan& inChunk) noexcept;
		
		static inline void skipWhile(ByteSpan &dc, const charset& cs) noexcept;
		static inline void skipUntil(ByteSpan &dc, const charset& cs) noexcept;
}



namespace pcore
{
	// skipWhile
	// Given a chunk, skip over characters that match the charset
	// until span is exhausted, or until a character not in the set is encountered
	//
	static inline void skipWhile(ByteSpan &dc, const charset& cs) noexcept
	{
		while (dc && cs.contains(*dc))
			dc++;
	}

	// skipUntil()
	// Given a chunk, skip over characters that are not in the charset
	// similar to chunk_token, but alters the chunk, rather than returning a new chunk
	// The input chunk will end up being blank if it reaches the end without
	// finding a character in the charset
	static inline void skipUntil(ByteSpan &dc, const charset& cs) noexcept
	{
		while (dc && !cs.contains(*dc))
			++dc;
	}


	// copy_to_cstr()
	// Copy the chunk to a c-string
	//
	static inline size_t copy_to_cstr(char* str, size_t len, const ByteSpan& a) noexcept
	{
		size_t maxBytes = chunk_size(a) < len ? chunk_size(a) : len;
		memcpy(str, a.fStart, maxBytes);
		str[maxBytes] = 0;

		return maxBytes;
	}

	// chunk_ltrim()
	// Trim the left side of skippable characters
	//
	static inline ByteSpan chunk_ltrim(const ByteSpan& a, const charset& skippable) noexcept
	{
		ByteSpan b = a;
		skipWhile(b, skippable);
		return b;

		//const uint8_t* start = a.fStart;
		//const uint8_t* end = a.fEnd;
		//while (start < end && skippable(*start))
		//	++start;
		//return { start, end };
	}

	// chunk_rtrim()
	// trim the right side of skippable characters
	//
	static inline ByteSpan chunk_rtrim(const ByteSpan& a, const charset& skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*(end - 1)))
			--end;

		return { start, end };
	}

	// chunk_trim()
	// trim the left and right side removing skippable characters
	//
	static inline ByteSpan chunk_trim(const ByteSpan& a, const charset& skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*start))
			++start;
		while (start < end && skippable(*(end - 1)))
			--end;
		return { start, end };
	}

	static inline ByteSpan chunk_skip_wsp(const ByteSpan& a) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && wspChars(*start))
			++start;
		return { start, end };
	}

	static inline ByteSpan chunk_subchunk(const ByteSpan& a, const size_t startAt, const size_t sz) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		if (startAt < chunk_size(a))
		{
			start += startAt;
			if (start + sz < end)
				end = start + sz;
			else
				end = a.fEnd;
		}
		else
		{
			start = end;
		}
		return { start, end };
	}

	static inline ByteSpan chunk_take(const ByteSpan& dc, size_t n) noexcept
	{
		return chunk_subchunk(dc, 0, n);
	}

	static inline bool chunk_starts_with(const ByteSpan& a, const ByteSpan& b) noexcept
	{
		return chunk_is_equal(chunk_subchunk(a, 0, chunk_size(b)), b);
	}

	static inline bool chunk_starts_with_char(const ByteSpan& a, const uint8_t b) noexcept
	{
		return chunk_size(a) > 0 && a.fStart[0] == b;
	}

	static inline bool chunk_starts_with_cstr(const ByteSpan& a, const char* b) noexcept
	{
		return chunk_starts_with(a, chunk_from_cstr(b));
	}

	static inline bool chunk_ends_with(const ByteSpan& a, const ByteSpan& b) noexcept
	{
		return chunk_is_equal(chunk_subchunk(a, chunk_size(a) - chunk_size(b), chunk_size(b)), b);
	}

	static inline bool chunk_ends_with_char(const ByteSpan& a, const uint8_t b) noexcept
	{
		return chunk_size(a) > 0 && a.fEnd[-1] == b;
	}

	static inline bool chunk_ends_with_cstr(const ByteSpan& a, const char* b) noexcept
	{
		return chunk_ends_with(a, chunk_from_cstr(b));
	}

	// chunk_token()
	// Given an input chunk
	// spit it into two chunks, 
	// Returns - the first chunk before delimeters
	// a - adjusted to reflect the rest of the input after delims
	// If delimeter NOT found
	// returns the entire input chunk
	// and 'a' is set to an empty chunk
	static inline ByteSpan chunk_token(ByteSpan& a, const charset& delims) noexcept
	{
		if (!a) {
			a = {};
			return {};
		}
		
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		const uint8_t* tokenEnd = start;
		while (tokenEnd < end && !delims(*tokenEnd))
			++tokenEnd;

		if (delims(*tokenEnd))
		{
			a.fStart = tokenEnd + 1;
		}
		else {
			a.fStart = tokenEnd;
		}

		return { start, tokenEnd };
	}

	// nextToken()
	//
	//static inline ByteSpan nextToken(ByteSpan& a, const charset&& delims) noexcept
	//{
	//	return chunk_token(a, delims);
	//}

	// chunk_find_char
	// Given an input chunk
	// find the first instance of a specified character
	// return the chunk preceding the found character
	// or or the whole chunk of the character is not found
	//
	static inline ByteSpan chunk_find_char(const ByteSpan& a, char c) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && *start != c)
			++start;

		return { start, end };
	}

	static inline ByteSpan chunk_find_cstr(const ByteSpan& a, const char* c) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		const uint8_t* cstart = (const uint8_t*)c;
		const uint8_t* cend = cstart + strlen(c);
		while (start < end && !chunk_starts_with({ start, end }, { cstart, cend }))
			++start;

		return { start, end };
	}

	// chunk_read_bracketed
	// Given a chunk, read the contents of the chunk until the matching close bracket is found
	// The chunk is advanced past the closing bracket
	//
	static inline ByteSpan chunk_read_bracketed(ByteSpan& src, const uint8_t lbracket, const uint8_t rbracket) noexcept
	{
		uint8_t* beginattrValue = nullptr;
		uint8_t* endattrValue = nullptr;
		uint8_t quote{};

		// Skip white space before the quoted bytes
		src = chunk_ltrim(src, wspChars);

		if (!src || *src != lbracket)
			return {};


		// advance past the lbracket, then look for the matching close quote
		src++;
		beginattrValue = (uint8_t*)src.fStart;

		// Skip until end of the value.
		while (src && *src != rbracket)
			src++;

		if (src)
		{
			endattrValue = (uint8_t*)src.fStart;
			src++;
		}

		// Store only well formed quotes
		return { beginattrValue, endattrValue };
	}
	

}


/*
	// Turn a chunk into a vector of chunks, splitting on the delimiters
    // return number of tokens found
    static inline  int chunk_split(const ByteSpan& inChunk, const charset& delims, std::vector<ByteSpan> &spans, bool wantEmpties = false) noexcept
    {
        ByteSpan s = inChunk;

        while (s)
        {
            ByteSpan token = chunk_token(s, delims);
            //if (size(token) > 0)
            spans.push_back(token);

        }

        return spans.size();
    }
	
*/

#include "bspanprint.h"
#include "bspanparse.h"

