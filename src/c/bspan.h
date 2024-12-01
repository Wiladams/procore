#ifndef BSPAN_H_INCLUDED
#define BSPAN_H_INCLUDED


#include <cstdint>
#include <cstddef>		// nullptr_t, ptrdiff_t, size_t
#include <cstring>



// C++ compatibility, ensure we use C naming instead of C++ name 
// mangling for these function names
//
#ifdef __cplusplus
extern "C" {
#endif
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



	// Forward function declarations

	// bspan_reset()
	// Reset the bspan to its initial empty state
	// same as: bspan_init(bs, nullptr, nullptr)
	static void bspan_reset(bspan * bs) noexcept;

	// bspan_init()
	// Initialize the bspan to point to a contiguous set of bytes with 
	// 'ptr1' and 'ptr2' as its endpoints.  Order does not matter.
	static int bspan_init(bspan * bs, const void *pt1, const void *pt2) noexcept;
	static int bspan_init_from_data(bspan * bs, const void *data, size_t sz) noexcept;
	static int bspan_init_from_cstr(bspan * bs, const char* cstr) noexcept;
	static int bspan_weak_assign(bspan *, const bspan *b) noexcept;

	static int bspan_set_all(bspan *bs, const unsigned char c) noexcept;
	static size_t bspan_copy_from_span(bspan * a, const bspan * b) noexcept;

	// bspan_size()
	// Return the number of contiguous bytes the span represents
	// Design: using ptrdiff_t instead of size_t, makes math with the return value
	// easier.  The size is guaranteed to be zero or greater
	static size_t bspan_size(const bspan *) noexcept;
	static bool bspan_is_empty(const bspan *) noexcept;
	static bool bspan_is_valid(const bspan *) noexcept;

	// setting up for a range-based for loop
	static const unsigned char* bspan_data(const bspan *) noexcept;
	static const unsigned char* bspan_begin(const bspan *) noexcept;
	static const unsigned char* bspan_end(const bspan *) noexcept;
	static unsigned char bspan_front(const bspan *) noexcept;

	// Comparison functions
	static bool bspan_begins_with_span(const bspan * a, const bspan * b) noexcept;
	static int bspan_compare_span(const bspan * a, const bspan * b) noexcept;


	// Common functions
	// bspan_reset()
	// Set the span to an unused state
	static void bspan_reset(bspan * bs) noexcept
	{
		bs->fStart = nullptr;
		bs->fEnd = nullptr;
	}


	// The span can be initialized with two pointers with their values
	// in any order.  The start will be the pointer with the lowest value,
	// and the end will be the pointer with the highest value.
	static int bspan_init(bspan * bs, const void *ptr1, const void *ptr2) noexcept
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
	static int bspan_init_from_cstr(bspan * bs, const char* cstr) noexcept 
	{ 
		return bspan_init(bs, cstr, cstr+strlen(cstr));
	}

	// bspan_init_from_data()
	// Initialize a span from any pointer/size provided
	static int bspan_init_from_data(bspan * bs, const void *data, size_t sz) noexcept
	{
		unsigned char *dataEnd = (unsigned char *)data + sz;
		return bspan_init(bs, data, dataEnd);
	}

	static int bspan_weak_assign(bspan *a, const bspan *b) noexcept
	{
		a->fStart = b->fStart;
		a->fEnd = b->fEnd;
		return 0;
	}

	// bspan_set_all()
	// set all bytes in the span to the specified value
	static int bspan_set_all(bspan *bs, const unsigned char c) noexcept
	{

		unsigned char *startAt = (unsigned char *)bs->fStart;
		const unsigned char *endAt = bs->fEnd;

		while (startAt < endAt)
		{
			*startAt = c;
			startAt++;
		}
	}

	// bspan_size()
	// Return the size of the bspan.  That is, the number of bytes
	// covered by the span.
	static size_t bspan_size(const bspan * bs) noexcept
	{
		if (nullptr == bs)
			return 0;

		return bs->fEnd - bs->fStart;
	}

	//bspan_is_empty()
	// Returns whether the bspan has a size of zero (empty)
	// This is purely a matter of convenience, since you can just
	// check the size directly
	static bool bspan_is_empty(const bspan * bs) noexcept {return bspan_size(bs)==0;}
	static bool bspan_is_valid(const bspan * bs) noexcept {return ((bs != nullptr) && (bs->fStart<bs->fEnd));}

	// For ranging
	static const unsigned char* bspan_data(const bspan * bs) noexcept {return bs->fStart;}
	static const unsigned char* bspan_begin(const bspan * bs) noexcept {return bs->fStart;}
	static const unsigned char* bspan_end(const bspan * bs) noexcept { return bs->fEnd; }
	static unsigned char bspan_front(const bspan *bs) noexcept {if ((bs->fStart != nullptr) && (bs->fStart < bs->fEnd)) return *bs->fStart; return 0; }
	
	// Byte Comparison operations
	static int bspan_compare_span(const bspan * a, const bspan * b) noexcept
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




	// bspan_subspan()
	//
	// Create a bytespan that is a subspan of another bytespan
	static  int bspan_subspan(const bspan * a, const size_t startAt, const size_t sz, bspan * b) noexcept
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

	// bspan_advance()
	//
	// move the start pointer forward on the span.  If the size requested 
	// would go past the end of the span, then don't do anything
	static int bspan_advance(bspan * a, size_t sz) noexcept		
	{
		if (a->fStart+sz > a->fEnd)
			return -1;

		a->fStart += sz;
			
		return 0;
	}

	// bspan_copy_from_span
	//
	// Copy as much of the 'b' span as will fit into the 'a' span
	// Return the number of bytes copied
	static size_t bspan_copy_from_span(bspan * a, const bspan * b) noexcept
	{
		size_t maxBytes = (bspan_size(a) < bspan_size(b)) ? bspan_size(a) : bspan_size(b);
		const unsigned char *srcAt = bspan_begin(b);
		unsigned char *dstAt = (unsigned char *)bspan_begin(a);

		for (int i=0;i<maxBytes;i++){
			*dstAt = *srcAt;
			dstAt++;
			srcAt++;
		}

		return maxBytes;
	}
#ifdef __cplusplus
}
#endif


#endif 	// BSPAN_H_INCLUDED
