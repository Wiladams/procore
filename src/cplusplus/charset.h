#pragma once

#include "asciiset.h"

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
			asciiset_add_set(&bits, &aset.bits);

			return *this;
		}

		// Methods for removing characters from the set
		CharSet& removeChar(const char achar)
		{
			asciiset_remove_char(&bits, achar);
			return *this;
		}
		
		CharSet& removeChars(const char* chars)
		{
			asciiset_remove_chars(&bits, chars);

			return *this;
		}
		
		CharSet& removeCharset(const CharSet& aset)
		{
			asciiset_remove_set(&bits, &aset.bits);
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
		bool contains(const unsigned char c) const { return asciiset_contains_char(&bits, c)==1;}
		bool operator [](const unsigned char c) const { return asciiset_contains_char(&bits, c)==1; }
		bool operator ()(const unsigned char c) const {return asciiset_contains_char(&bits, c)==1;}
		
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
