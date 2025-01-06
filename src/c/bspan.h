#ifndef BSPAN_H_INCLUDED
#define BSPAN_H_INCLUDED



#include "pcoredef.h"



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
	INLINE void bspan_reset(bspan * bs) PC_NOEXCEPT_C;

	// bspan_init()
	// Initialize the bspan to point to a contiguous set of bytes with 
	// 'ptr1' and 'ptr2' as its endpoints.  Order does not matter.
	INLINE int bspan_init(bspan *bs) PC_NOEXCEPT_C;
	INLINE int bspan_init_from_pointers(bspan * bs, const void *pt1, const void *pt2) PC_NOEXCEPT_C;
	INLINE int bspan_init_from_data(bspan * bs, const void *data, size_t sz) PC_NOEXCEPT_C;
	INLINE int bspan_init_from_cstr(bspan * bs, const char* cstr) PC_NOEXCEPT_C;
	INLINE int bspan_set_begin(bspan *, const void *pt) PC_NOEXCEPT_C;
	INLINE int bspan_set_end(bspan *, const void *) PC_NOEXCEPT_C;

	INLINE int bspan_init_copy(bspan *, const bspan *b) PC_NOEXCEPT_C;
	INLINE int bspan_shallow_equal(const bspan *, const bspan *) PC_NOEXCEPT_C;

	INLINE int bspan_set_all(bspan *bs, const unsigned char c) PC_NOEXCEPT_C;
	INLINE size_t bspan_copy_from_span(bspan * a, const bspan * b) PC_NOEXCEPT_C;

	// bspan_size()
	// Return the number of contiguous bytes the span represents
	// Design: using ptrdiff_t instead of size_t, makes math with the return value
	// easier.  The size is guaranteed to be zero or greater
	INLINE size_t bspan_size(const bspan *) PC_NOEXCEPT_C;
	INLINE bool bspan_is_empty(const bspan *) PC_NOEXCEPT_C;
	INLINE bool bspan_is_valid(const bspan *) PC_NOEXCEPT_C;

	// setting up for a range-based for loop
	INLINE const unsigned char* bspan_data(const bspan *) PC_NOEXCEPT_C;
	INLINE const unsigned char* bspan_begin(const bspan *) PC_NOEXCEPT_C;
	INLINE const unsigned char* bspan_end(const bspan *) PC_NOEXCEPT_C;
	INLINE unsigned char bspan_front(const bspan *) PC_NOEXCEPT_C;

	INLINE int bspan_advance(bspan * a, size_t sz) PC_NOEXCEPT_C;
	INLINE int bspan_slide(bspan *a, int offset) PC_NOEXCEPT_C;

	// Common functions
	// bspan_reset()
	// Set the span to an unused state
	INLINE void bspan_reset(bspan * bs) PC_NOEXCEPT_C
	{
		bs->fStart = nullptr;
		bs->fEnd = nullptr;
	}


	// The span can be initialized with two pointers with their values
	// in any order.  The start will be the pointer with the lowest value,
	// and the end will be the pointer with the highest value.
	INLINE int bspan_init(bspan *bs) PC_NOEXCEPT_C
	{
		return bspan_init_from_pointers(bs, nullptr, nullptr);
	}

	INLINE int bspan_init_from_pointers(bspan * bs, const void *ptr1, const void *ptr2) PC_NOEXCEPT_C
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
	INLINE int bspan_init_from_cstr(bspan * bs, const char* cstr) PC_NOEXCEPT_C
	{ 
		return bspan_init_from_pointers(bs, cstr, cstr+strlen(cstr));
	}

	// bspan_init_from_data()
	// Initialize a span from any pointer/size provided
	INLINE int bspan_init_from_data(bspan * bs, const void *data, size_t sz) PC_NOEXCEPT_C
	{
		unsigned char *dataEnd = (unsigned char *)data + sz;
		return bspan_init_from_pointers(bs, data, dataEnd);
	}

	INLINE int bspan_set_begin(bspan *a, const void *pt) PC_NOEXCEPT_C
{
	if (pt > a->fEnd)
		return -1;

	a->fStart = (const unsigned char *)pt;
	return 0;
}

	INLINE int bspan_set_end(bspan *a, const void *pt) PC_NOEXCEPT_C
{
	if (pt < a->fStart)
		return -1;

	a->fEnd = (const unsigned char *)pt;
	return 0;
}

	INLINE int bspan_init_copy(bspan *a, const bspan *b) PC_NOEXCEPT_C
	{
		if ((NULL == a) || (NULL == b))
			return -1;
		
		a->fStart = b->fStart;
		a->fEnd = b->fEnd;
		
		return 0;
	}

	INLINE int bspan_shallow_equal(const bspan *a, const bspan *b) PC_NOEXCEPT_C
	{
		return (a->fStart == b->fStart) && (a->fEnd == b->fEnd);
	}

	// bspan_set_all()
	// set all bytes in the span to the specified value
	INLINE int bspan_set_all(bspan *bs, const unsigned char c) PC_NOEXCEPT_C
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
	INLINE size_t bspan_size(const bspan * bs) PC_NOEXCEPT_C
	{
		if (nullptr == bs)
			return 0;

		return bs->fEnd - bs->fStart;
	}

	//bspan_is_empty()
	// Returns whether the bspan has a size of zero (empty)
	// This is purely a matter of convenience, since you can just
	// check the size directly
	INLINE bool bspan_is_empty(const bspan * bs) PC_NOEXCEPT_C {return bspan_size(bs)==0;}
	INLINE bool bspan_is_valid(const bspan * bs) PC_NOEXCEPT_C {return ((bs != nullptr) && (bs->fStart<bs->fEnd));}

	// For ranging
	INLINE const unsigned char* bspan_data(const bspan * bs) PC_NOEXCEPT_C {return bs->fStart;}
	INLINE const unsigned char* bspan_begin(const bspan * bs) PC_NOEXCEPT_C {return bs->fStart;}
	INLINE const unsigned char* bspan_end(const bspan * bs) PC_NOEXCEPT_C { return bs->fEnd; }
	INLINE unsigned char bspan_front(const bspan *bs) PC_NOEXCEPT_C {if ((bs->fStart != nullptr) && (bs->fStart < bs->fEnd)) return *bs->fStart; return 0; }
	INLINE unsigned char bspan_back(const bspan *bs) PC_NOEXCEPT_C {if (bspan_is_valid(bs) && (bs->fEnd > bs->fStart) ) return *(bs->fEnd-1); return 0;}
	





	// bspan_subspan()
	//
	// Create a bytespan that is a subspan of another bytespan
	INLINE  int bspan_subspan(const bspan * a, const size_t startAt, const size_t sz, bspan * b) PC_NOEXCEPT_C
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

		return bspan_init_from_pointers(b, astart, end );
	}

	// bspan_advance()
	//
	// move the start pointer forward on the span.  If the size requested 
	// would go past the end of the span, then don't do anything
	INLINE int bspan_advance(bspan * a, size_t sz) PC_NOEXCEPT_C
	{
		if (a->fStart+sz > a->fEnd)
			return -1;

		a->fStart += sz;
			
		return 0;
	}

	// bspan_slide()
	//
	// Move the sart pointer, and the end pointer by the same amount
	// We assume we can't move the start pointer less than zero
	// we don't make any assumption about how high the end pointer can go
	// perhaps it should stop at UINT_MAX
	
	INLINE int bspan_slide(bspan *a, int offset) PC_NOEXCEPT_C
	{
		if (offset < 0)
		{
			if ((a->fStart + offset) < (unsigned char*)0)
				return -1;
		}

		a->fStart += offset;
		a->fEnd += offset;

		return 0;
	}


	// bspan_copy_from_span
	//
	// Copy as much of the 'b' span as will fit into the 'a' span
	// Return the number of bytes copied
	INLINE size_t bspan_copy_from_span(bspan * a, const bspan * b) PC_NOEXCEPT_C
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
