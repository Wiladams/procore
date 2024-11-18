#ifndef BSPAN_H
#define BSPAN_H

//
// A core type for representing a contiguous sequence of bytes
// As of C++ 20, there is std::span, but it is not yet widely supported
// 
// The ByteSpan is used in everything from networking
// to graphics bitmaps to audio buffers.
// Having a universal representation of a chunk of data
// allows for easy interoperability between different
// subsystems.  
// 
// It also allows us to eliminate disparate implementations that
// are used for the same purpose.

// Implementation note: The ByteSpan is a 'view' of a chunk of data.
// We are using a begin/end pointer pair to represent
// the view.

#include <cstdint>
#include <cstring>
#include <iterator>	// for std::data(), std::size()


#include "pconfig.hpp"



	// bspan
	//
	// A structure that respresents a set of contiguous bytes
	// The bspan does not 'own' the memory in any way, that is completely 
	// handled by the application that is using the bspan.
	// The bspan provides ready access to those bytes, facilitating various
	// type conversions, and stream like interfaces.
	// There are two different ways in which this structure could be represented:
	// 1) startPtr, endPtr
	// 2) offset, size
	// Both representations would require two pointer sized fields to 
	// represent a full memory range, so it really is just a matter
	// of convenience to do it one way or another.
	// Calling bspan_size(), bspan_start(), bspan_end(), are probably the most
	// common operations.
	//
	struct bspan_t 
	{
		const unsigned char* fStart;
		const unsigned char* fEnd;
	};

	typedef struct bspan_t bspan;
	typedef struct bspan_t* bspan_p;

// C++ compatibility, ensure we use C naming instead of C++ name 
// mangling for these function names
//
#ifdef __cplusplus
extern "C" {
#endif
	// Forward function declarations
	// bspan_reset()
	// Reset the bspan to its initial empty state
	// same as: bspan_init(bs, nullptr, nullptr)
	static void bspan_reset(bspan_p bs) noexcept;

	// bspan_init()
	// Initialize the bspan to point to a contiguous set of bytes with 
	// 'ptr1' and 'ptr2' as its endpoints.  Order does not matter.
	static int bspan_init(bspan_p bs, const void *pt1, const void *pt2) noexcept;
	static int bspan_init_from_data(bspan_p bs, const void *data, size_t sz) noexcept;
	static int bspan_init_from_cstr(bspan_p bs, const char* cstr) noexcept;
	
	// bspan_size()
	// Return the number of contiguous bytes the span represents
	// Design: using ptrdiff_t instead of size_t, makes math with the return value
	// easier.  The size is guaranteed to be zero or greater
	static ptrdiff_t bspan_size(bspan_p) noexcept;
	static bool bspan_is_empty(bspan_p) noexcept;

	// setting up for a range-based for loop
	static const unsigned char* bspan_data(const bspan_p) noexcept;
	static const unsigned char* bspan_begin(const bspan_p) noexcept;
	static const unsigned char* bspan_end(const bspan_p) noexcept;

	// Comparison functions
	static bool bspan_begins_with_span(const bspan_p a, const bspan_p b) noexcept;
	static int bspan_compare_span(const bspan_p a, const bspan_p b) noexcept;


	// Common functions
	// bspan_reset()
	// Set the span to an unused state
	static void bspan_reset(bspan_p bs) noexcept
	{
		bs->fStart = nullptr;
		bs->fEnd = nullptr;
	}


	// The span can be initialized with two pointers with their values
	// in any order.  The start will be the pointer with the lowest value,
	// and the end will be the pointer with the highest value.
	static int bspan_init(bspan_p bs, const void *ptr1, const void *ptr2) noexcept
	{
		if (ptr1<ptr2){
			bs->fStart = (unsigned char *)ptr1;
			bs->fEnd = (unsigned char *)ptr2;
		} else {
			bs->fStart = (unsigned char *)ptr2;
			bs->fEnd = (unsigned char *)ptr1;
		}

		return 0;
	}

	// bspan_init_from_cstr
	// Initilize a span from a 'C' sring (classic null terminated)
	static int bspan_init_from_cstr(bspan_p bs, const char* cstr) noexcept 
	{ 
		return bspan_init(bs, cstr, cstr+strlen(cstr));
	}

	// bspan_init_from_data()
	// Initialize a span from any pointer/size provided
	static int bspan_init_from_data(bspan_p bs, const void *data, size_t sz) noexcept
	{
		unsigned char *dataEnd = (unsigned char *)data + sz;
		return bspan_init(bs, data, dataEnd);
	}

	static ptrdiff_t bspan_size(const bspan_p bs) noexcept
	{
		return bs->fEnd - bs->fStart;
	}

	static bool bspan_is_empty(const bspan_p bs) noexcept {return bspan_size(bs)==0;}

	// For ranging
	static const unsigned char* bspan_data(const bspan_p bs) noexcept {return bs->fStart;}
	static const unsigned char* bspan_begin(const bspan_p bs) noexcept {return bs->fStart;}
	static const unsigned char* bspan_end(const bspan_p bs) noexcept { return bs->fEnd; }

	// Byte Comparison operations
	static int bspan_compare_span(const bspan_p a, const bspan_p b) noexcept
	{
		const unsigned char *cs = bspan_begin(a);
		const unsigned char *ct = bspan_begin(b);

		ptrdiff_t maxN = bspan_size(a) < bspan_size(b) ? bspan_size(a) : bspan_size(b);

		for (int i=0;i<maxN;i++,cs++,ct++)
		{
			if (*cs < *ct)
				return -1;
			if (*ct > *ct)
				return 1;
		}

		// We've gotten through the loop
		// so thus far, all bytes are the same
		// we can further constrain by seeing if we've
		// actually exhausted all available bytes in one span
		// or the other.  The one that is short, is the lesser one
		if (bspan_size(a) == bspan_size(b))
			return 0;

		// The 'a' span is shorter, so it is the 'lesser'
		// of the two spans
		if (bspan_size(a) < bspan_size(b))
			return -1;

		return 1;
	}


	// bspan_begins_with_bspan()
	// This is the first one with the cleanest semantics
	// It is a byte comparison, assuming the first operand is at 
	// least as big as the second, and that every byte from the second
	// operand is located in sequence in the first operand.
	static bool bspan_begins_with_span(const bspan_p a, const bspan_p b) noexcept
	{
		ptrdiff_t len = bspan_size(b);
		
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

// Create a bytespan that is a subspan of another bytespan
	static  int bspan_subspan(const bspan_p a, const size_t startAt, const size_t sz, bspan_p b) noexcept
	{
		const unsigned char* astart = bspan_begin(a);
		const unsigned char* end = bspan_end(a);
		if (startAt < bspan_size(a))
		{
			astart += startAt;
			if (astart + sz < end)
				end = astart + sz;
			else
				end = bspan_end(a);
		}
		else
		{
			astart = end;
		}

		return bspan_init(b, astart, end );
	}

	static int bspan_advance(bspan_p a, size_t sz, bspan_p b) noexcept		
	{
		return bspan_subspan(a, 1,bspan_size(a), b);
	}
#ifdef __cplusplus
}
#endif

// Place C++ operators and other things in here
#ifdef __cplusplus
	// get current value from fStart, like a 'peek' operation
	static unsigned char& operator*(bspan &a) { static unsigned char zero = 0;  if (a.fStart < a.fEnd) return *(unsigned char*)a.fStart; return  zero; }
	static const uint8_t& operator*(const bspan &a) { static unsigned char zero = 0;  if (a.fStart < a.fEnd) return *(unsigned char*)a.fStart; return  zero; }

#endif


#endif 	// BSPAN_H
