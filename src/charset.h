#ifndef CHARSET_H_INCLUDED
#define CHARSET_H_INCLUDED



// Represent a set of characters as a bitset
//
// Typical usage:
//   charset whitespaceChars("\t\n\f\r ");
//
//	 // skipping over whitespace
//   while (whitespaceChars[c])
//		c = nextChar();
//
//
//  This is better than simply using the old classic isspace() and other functions
//  as you can define your own sets, depending on your needs:
//
//  charset delimeterChars("()<>[]{}/%");
//
//  Of course, there will surely be a built-in way of doing this in C/C++ 
//  and it will no doubt be tied to particular version of the compiler.  Use that
//  if it suits your needs.  Meanwhile, at least you can see how such a thing can
//  be implemented.


#include <memory>
#include "bithacks.h"

#ifdef __cplusplus
extern "C" {
#endif


// The asciiset_t is essentially a bitset of values from the ASCII table.
// This set is used to collect characters which can be tested against, typically
// in tokenization tasks.  For example, an asciiset of whitespace characters;
//     asciiset aset;
//     asciiset_init_from_cstr(&aset, " \t\r\n");
// ASCII CONTROL	0x00 : 0x15
// ASCII PRINTABLE 	0x16 : 0x7f
// ASCII EXTENDED	0x80 : 0xff

#define ASCIISET_SIZE 32

struct asciiset_t {
	unsigned char fBits[ASCIISET_SIZE];
};
typedef struct asciiset_t asciiset;

static void asciiset_reset(asciiset *) noexcept;
static int asciiset_init_from_char(asciiset *, const char ) noexcept;
static int asciiset_init_from_cstr(asciiset *, const char *cstr) noexcept;

static int asciiset_add_cstr(asciiset *, const char *cstr) noexcept;
static int asciiset_add_asciiset(asciiset *, const asciiset *) noexcept;

static int asciiset_get_bit(const asciiset *cs, unsigned char c) noexcept;
static int asciiset_set_bit(const asciiset *cs, unsigned char c) noexcept;
static int asciiset_remove_bit(asciiset *cs, const unsigned char c) noexcept;


// Implementation
static void asciiset_reset(asciiset *cs) noexcept
{
	// set all bytes to zero
	memset(cs->fBits, 0, ASCIISET_SIZE);
}

static int asciiset_init_from_char(asciiset *cs, const char c) noexcept
{
	asciiset_reset(cs);
	return asciiset_set_bit(cs, c);
}

static int asciiset_init_from_cstr(asciiset *cs, const char *cstr) noexcept
{
	asciiset_reset(cs);
	asciiset_add_cstr(cs, cstr);

	return 0;
}

static int asciiset_get_bit(const asciiset *cs, unsigned char c) noexcept
{
	return bhak_get_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_set_bit(asciiset *cs, const unsigned char c) noexcept
{
	return bhak_set_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_remove_bit(asciiset *cs, const unsigned char c) noexcept
{
	return bhak_remove_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_add_cstr(asciiset *cs, const char *cstr) noexcept
{
	char *str = (char *)cstr;
	while (*str)
	{
		bhak_set_bit_value(cs->fBits, ASCIISET_SIZE, *str);
		str++;
	}
}

static int asciiset_add_asciiset(asciiset *a, const asciiset *b) noexcept
{
	for (int i=0;i< ASCIISET_SIZE;i++)
		a->fBits[i] |= b->fBits[i];
}



#ifdef __cplusplus
}
#endif

namespace pcore {

	struct CharSet {
		asciiset bits;

		// Common Constructors
		CharSet(const char achar) { addChar(achar); }
		CharSet(const char* chars) { addChars(chars); }
		CharSet(const CharSet& aset) { addCharset(aset); }

		
		// Convenience methods for adding and removing characters
		// in the set
		
		// Add a single character to the set
		CharSet& addChar(const char achar)
		{
			asciiset_add_char(&bits, achar);
			return *this;
		}

		// Add a range of characters to the set
		CharSet& addChars(const char* cstr)
		{
			asciiset_add_cstr(&bits, cstr);
			return *this;
		}

		CharSet& addCharset(const CharSet& aset)
		{
			asciiset_add_asciiset(&bits, &aset.bits);

			return *this;
		}

		// Methods for removing characters from the set
		CharSet& removeChar(const char achar)
		{
			bits.reset(achar);
			return *this;
		}
		
		CharSet& removeChars(const char* chars)
		{
			const char* s = chars;
			while (0 != *s)
				bits.reset(*s++);
			return *this;
		}
		
		CharSet& removeCharset(const CharSet& aset)
		{
			bits &= ~aset.bits;
			return *this;
		}
		
		// Convenience for adding characters and strings
		CharSet& operator+=(const char achar) { return addChar(achar); }
		CharSet& operator+=(const char* chars) { return addChars(chars); }
		CharSet& operator+=(const CharSet& aset) { return addCharset(aset); }
		
		// Creating a new set
		CharSet operator+(const char achar) const { CharSet result(*this); return result.addChar(achar); }
		CharSet operator+(const char* chars) const { CharSet result(*this); return result.addChars(chars); }
		CharSet operator+(const CharSet& aset) const { CharSet result(*this); return result.addCharset(aset); }
		
		// create operator- to remove a character from a set
		CharSet operator-(const char achar) const { CharSet result(*this); result.removeChar(achar); return result; }
		CharSet operator-(const char* chars) const { CharSet result(*this); result.removeChars(chars); return result; }
		CharSet operator-(const CharSet& aset) const { CharSet result(*this); result.removeCharset(aset); return result; }

		CharSet& operator-=(const char achar) { return removeChar(achar); }
		CharSet& operator-=(const char* chars) { return removeChars(chars); }

		
		// Checking for set membership
		bool contains(const unsigned char c) const { return asciiset_get_bit(&bits, c)==1;}
		bool operator [](const unsigned char c) const { return asciiset_get_bit(&bits, c)==1; }
		bool operator ()(const unsigned char c) const {return asciiset_get_bit(&bits, c)==1;}
		
		// create operator^ to create a new set that is the union of two sets
		// The addition operator is good enough, we don't need these additional methods
		//CharSet operator^(const CharSet& other) const { charset result(*this); result.bits |= other.bits; return result; }
		//CharSet& operator^=(const CharSet& other) { bits |= other.bits; return *this; }
		
	};
}

// Some useful character sets
namespace pcore {
	static CharSet digitChars("0123456789");
	static CharSet hexChars("0123456789abcdefABCDEF");
	static CharSet octalChars("01234567");
	static CharSet binChars("01");

	static CharSet wspChars(" \r\n\t\f\v");		// a set of typical whitespace chars
	
	//	static CharSet alphaChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	//	static CharSet alphaNumChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");



}
