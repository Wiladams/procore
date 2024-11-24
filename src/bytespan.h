	#pragma once

    #include "bspan.h"

namespace pcore {
    struct ByteSpan
	{
	private:
		bspan fSpan{};

public:
		// Constructors
		ByteSpan() = default;
		ByteSpan(const unsigned char* astart, const unsigned char* aend)
		{
			bspan_init(&fSpan, astart, aend);
		}
		ByteSpan(const char* cstr)
		{
			bspan_init_from_cstr(&fSpan, cstr);
		}
		explicit ByteSpan(const void* data, size_t sz) 
		{
			bspan_init_from_data(&fSpan, data, sz);
		}

		// setting up for a range-based for loop
		const unsigned char* data() const noexcept { return (unsigned char*)bspan_data(&fSpan); }
		const unsigned char* begin() const noexcept { return bspan_begin(&fSpan); }
		const unsigned char* end() const noexcept { return bspan_end(&fSpan); }
		size_t size()  const noexcept { return bspan_size(&fSpan); }
		const bool empty() const noexcept { return bspan_is_empty(&fSpan); }



		// Type conversions
		// the type 'bool' is used for convenience in loop termination
		explicit operator bool() const { return bspan_is_valid(&fSpan); };


		// Array access
		unsigned char& operator[](size_t i) { return ((unsigned char*)begin())[i]; }
		const unsigned char& operator[](size_t i) const { return ((unsigned char*)begin())[i]; }

		// get current value from fStart, like a 'peek' operation
		unsigned char& operator*() { static unsigned char zero = 0;  if (begin() < end()) return *(unsigned char*)begin(); return  zero; }
		const uint8_t& operator*() const { static unsigned char zero = 0;  if (fSpan.fStart < fSpan.fEnd) return *(unsigned char*)fSpan.fStart; return  zero; }

		ByteSpan& operator+= (size_t n) {
			bspan_advance(&fSpan, n);

			return *this;
		}


		ByteSpan& operator++() { return operator+=(1); }			// prefix notation ++y
		ByteSpan& operator++(int i) { return operator+=(1); }       // postfix notation y++

		bool operator==(const ByteSpan& b) noexcept
		{
			return (bspan_compare_span(&fSpan, &b.fSpan) == 0);
		}

		bool operator==(const char* b) noexcept
		{
			bspan cspan;
			bspan_init_from_cstr(&cspan, b);
			return (bspan_compare_span(&fSpan, &cspan) == 0);
		}

		bool operator!=(const ByteSpan& b) noexcept
		{
			if (size() != b.size())
				return true;

			return !this->operator==(b);
		}

		bool operator<(const ByteSpan& b) noexcept
		{
			return bspan_compare_span(&fSpan, &b.fSpan) < 0;
		}

		bool operator>(const ByteSpan& b) noexcept
		{
			return bspan_compare_span(&fSpan, &b.fSpan) > 0;
		}

		bool operator<=(const ByteSpan& b) noexcept
		{
			return bspan_compare_span(&fSpan, &b.fSpan) <= 0;
		}

		bool operator>=(const ByteSpan& b) noexcept
		{
			return bspan_compare_span(&fSpan, &b.fSpan) >= 0;
		}


		void setAll(unsigned char c) noexcept { bspan_set_all(&fSpan, c); }
		
		size_t copyFrom(const ByteSpan &b)
		{
			return bspan_copy_from_span(&fSpan, &b.fSpan);
		}

		size_t copyFrom(const char *b)
		{
			bspan cspan;
			bspan_init_from_cstr(&cspan, b);
			return bspan_copy_from_span(&fSpan, &cspan);
		}
	};


}
                       
// Implementation of hash function for ByteSpan
// so it can be used in 'map' collections
namespace std {
	template<>
	struct hash<pcore::ByteSpan> {
		size_t operator()(const pcore::ByteSpan& span) const {
			uint32_t hash = 0;
			
			for (const unsigned char* p = span.fStart; p != span.fEnd; ++p) {
				hash = hash * 31 + *p;
			}
			return hash;
		}
	};
}

/*
	static inline int compare(const ByteSpan& a, const ByteSpan& b) noexcept
	{
		size_t maxBytes = a.size() < b.size() ? a.size() : b.size();
		return memcmp(a.fStart, b.fStart, maxBytes);
	}

	static inline int comparen(const ByteSpan& a, const ByteSpan& b, int n) noexcept
	{
		size_t maxBytes = a.size() < b.size() ? a.size() : b.size();
		if (maxBytes > n)
			maxBytes = n;
		return memcmp(a.fStart, b.fStart, maxBytes);
	}

	static inline int comparen_cstr(const ByteSpan& a, const char* b, int n) noexcept
	{
		size_t maxBytes = a.size() < n ? a.size() : n;
		return memcmp(a.fStart, b, maxBytes);
	}
*/

		/*
		//
		// Note:  For the various 'as_xxx' routines, there is no size
		// error checking.  It is assumed that whatever is calling the
		// function will do appropriate error checking beforehand.
		// This is a little unsafe, but allows the caller to decide where
		// they want to do the error checking, and therefore control the
		// performance characteristics better.

		// Read a single byte
		uint8_t as_u8()  noexcept
		{
			uint8_t result = *((uint8_t*)fStart);

			return result;
		}

		// Read a unsigned 16 bit value
		// assuming stream is in little-endian format
		// and machine is also little-endian
		uint16_t as_u16_le() noexcept
		{
			uint16_t r = *((uint16_t*)fStart);

			return r;

			//return ((uint8_t *)fStart)[0] | (((uint8_t *)fStart)[1] << 8);
		}

		// Read a unsigned 32-bit value
		// assuming stream is in little endian format
		uint32_t as_u32_le() noexcept
		{
			uint32_t r = *((uint32_t*)fStart);


			return r;

			//return ((uint8_t *)fStart)[0] | (((uint8_t *)fStart)[1] << 8) | (((uint8_t *)fStart)[2] << 16) |(((uint8_t *)fStart)[3] << 24);
		}

		// Read a unsigned 64-bit value
		// assuming stream is in little endian format
		uint64_t as_u64_le() noexcept
		{
			uint64_t r = *((uint64_t*)fStart);

			return r;
			//return ((uint8_t *)fStart)[0] | (((uint8_t *)fStart)[1] << 8) | (((uint8_t *)fStart)[2] << 16) | (((uint8_t *)fStart)[3] << 24) |
			//    (((uint8_t *)fStart)[4] << 32) | (((uint8_t *)fStart)[5] << 40) | (((uint8_t *)fStart)[6] << 48) | (((uint8_t *)fStart)[7] << 56);
		}

		//=============================================
		// BIG ENDIAN
		//=============================================
		// Read a unsigned 16 bit value
		// assuming stream is in big endian format
		uint16_t as_u16_be() noexcept
		{
			uint16_t r = *((uint16_t*)fStart);
			return bswap16(r);
		}

		// Read a unsigned 32-bit value
		// assuming stream is in big endian format
		uint32_t as_u32_be() noexcept
		{
			uint32_t r = *((uint32_t*)fStart);
			return bswap32(r);
		}

		// Read a unsigned 64-bit value
		// assuming stream is in big endian format
		uint64_t as_u64_be() noexcept
		{
			uint64_t r = *((uint64_t*)fStart);
			return bswap64(r);
		}
*/