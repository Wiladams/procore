#pragma once

#include "pconfig.hpp"
#include "bspan.h"

namespace pcore
{
	// simple type parsing
	static inline int64_t toInteger(const ByteSpan& inChunk) noexcept;
	static inline double toNumber(const ByteSpan& inChunk) noexcept;
	static inline std::string toString(const ByteSpan& inChunk) noexcept;
	static inline int toBoolInt(const ByteSpan& inChunk) noexcept;
}

namespace pcore {

	// Take a chunk containing a series of digits and turn
	// it into a 64-bit unsigned integer
	// Stop processing when the first non-digit is seen, 
	// or the end of the chunk
	static inline uint64_t chunk_to_u64(ByteSpan& s) noexcept
	{

		uint64_t v = 0;

		while (s && digitChars(*s))
		{
			v = v * 10 + (uint64_t)(*s - '0');
			s++;
		}

		return v;
	}

	// chunk_to_i64
	// Take a chunk containing a series of digits and turn
	// it into a 64-bit signed integer
	static inline int64_t chunk_to_i64(ByteSpan& s) noexcept
	{

		int64_t v = 0;

		bool negative = false;
		if (s && *s == '-')
		{
			negative = true;
			s++;
		}

		while (s && digitChars(*s))
		{
			v = v * 10 + (int64_t)(*s - '0');
			s++;
		}

		if (negative)
			v = -v;

		return v;
	}

	// Scan a number, which may have units after it.
	// Return the span that represents the numeric portion
	//   1.2em
	// -1.0E2em
	// 2.34ex
// -2.34e3M10,20
// 
// By the end of this routine, the numchunk represents the range of the 
// captured number.
// 
// The returned chunk represents what comes next, and can be used
// to continue scanning the original inChunk
//
// Note:  We assume here that the inChunk is already positioned at the start
// of a number (including +/- sign), with no leading whitespace

	static ByteSpan scanNumber(const ByteSpan& inChunk, ByteSpan& numchunk) noexcept
	{

		ByteSpan s = inChunk;
		numchunk = inChunk;
		numchunk.fEnd = inChunk.fStart;


		// sign
		if (*s == '-' || *s == '+') {
			s++;
			numchunk.fEnd = s.fStart;
		}

		// integer part
		while (s && digitChars[*s]) {
			s++;
			numchunk.fEnd = s.fStart;
		}

		if (*s == '.') {
			// decimal point
			s++;
			numchunk.fEnd = s.fStart;

			// fraction part
			while (s && digitChars[*s]) {
				s++;
				numchunk.fEnd = s.fStart;
			}
		}

		// exponent
		// but it could be units (em, ex)
		if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x'))
		{
			s++;
			numchunk.fEnd = s.fStart;

			// Might be a sign
			if (*s == '-' || *s == '+') {
				s++;
				numchunk.fEnd = s.fStart;
			}

			// Get any remaining digits
			while (s && digitChars[*s]) {
				s++;
				numchunk.fEnd = s.fStart;
			}
		}

		return s;
	}

	// Return a power of 10 using a lookup table
	// works with exponents from 0 to 22
	static inline double pow10(size_t n) noexcept
	{
		static const double p10[] = {
			1e0, 1e1, 1e2, 1e3, 1e4,
			1e5, 1e6, 1e7, 1e8, 1e9,
			1e10, 1e11, 1e12, 1e13, 1e14,
			1e15, 1e16, 1e17, 1e18, 1e19,
			1e20, 1e21, 1e22
		};

		return p10[n];
	}

	// parse floating point number
	// includes sign, exponent, and decimal point
	// The input chunk is altered, with the fStart pointer moved to the end of the number
	static inline double chunk_to_double(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;

		double sign = 1.0;
		double res = 0.0;
		long long intPart = 0;
		uint64_t fracPart = 0;
		bool hasIntPart = false;
		bool hasFracPart = false;

		// Parse optional sign
		if (*s == '+') {
			s++;
		}
		else if (*s == '-') {
			sign = -1;
			s++;
		}

		// Parse integer part
		if (digitChars[*s]) {

			intPart = chunk_to_u64(s);

			res = (double)intPart;
			hasIntPart = true;
		}

		// Parse fractional part.
		if (*s == '.') {
			s++; // Skip '.'
			auto sentinel = s.fStart;

			if (digitChars(*s)) {
				fracPart = chunk_to_u64(s);
				auto ending = s.fStart;

				ptrdiff_t diff = ending - sentinel;
				//res = res + ((double)fracPart) / (double)powd((double)10, double(diff));
				res = res + ((double)fracPart) / (double)pow10(diff);
				hasFracPart = true;
			}
		}

		// A valid number should have integer or fractional part.
		if (!hasIntPart && !hasFracPart)
			return 0.0;


		// Parse optional exponent
		if (*s == 'e' || *s == 'E') {
			long long expPart = 0;
			s++; // skip 'E'

			double expSign = 1.0;
			if (*s == '+') {
				s++;
			}
			else if (*s == '-') {
				expSign = -1.0;
				s++;
			}

			if (digitChars[*s]) {
				expPart = chunk_to_u64(s);
				res = res * (1.0/pow10(expPart));
			}
		}

		return res * sign;
	}

	// parseNextNumber
	// parseNextNumber(ByteSpan &s, double &outNumber)
	//
	// Consume the next number off the front of the chunk
	// modifying the input chunk to advance past the  number
	// we removed.
	// Return true if we found a number, false otherwise
	static inline bool parseNextNumber(ByteSpan& s, double& outNumber)
	{
		static charset numWspChars(",\t\n\f\r ");          // whitespace found in paths

		// clear up leading whitespace, including ','
		s = chunk_ltrim(s, numWspChars);

		// If chunk is blank, return false 
		if (!s)
			return false;

		ByteSpan numChunk{};
		s = scanNumber(s, numChunk);

		if (!numChunk)
			return false;

		outNumber = toNumber(numChunk);

		return true;
	}

	// toInteger()
    // Return a signed integer from a chunk
    //
	static inline int64_t toInteger(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;
		return chunk_to_i64(s);
	}

	// toNumber()
	// a floating point number
    //
	static inline double toNumber(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;
		return chunk_to_double(s);
	}

	// toBoolInt()
    // return 1 if the chunk is "true" or "1" or "t" or "T" or "y" or "Y" or "yes" or "Yes" or "YES"
	// return 0 if the chunk is "false" or "0" or "f" or "F" or "n" or "N" or "no" or "No" or "NO"
	// return 0 otherwise
	static inline int toBoolInt(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;

		if (s == "true" || s == "1" || s == "t" || s == "T" || s == "y" || s == "Y" || s == "yes" || s == "Yes" || s == "YES")
			return 1;
		else if (s == "false" || s == "0" || s == "f" || s == "F" || s == "n" || s == "N" || s == "no" || s == "No" || s == "NO")
			return 0;
		else
			return 0;
	}

    // toString()
    // 
	static inline std::string toString(const ByteSpan& inChunk) noexcept
	{
		return std::string(inChunk.fStart, inChunk.fEnd);
	}

}
