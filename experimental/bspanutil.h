#pragma once


#include "bspan.h"
#include "charset.h"


namespace pcore
{
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
	

}


