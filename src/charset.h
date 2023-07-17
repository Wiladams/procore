#pragma once

#include "pconfig.hpp"
#include <bitset>

namespace pcore {

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
	struct charset {
		std::bitset<256> bits;

		// Common Constructors
		charset(const char achar) { addChar(achar); }
		charset(const char* chars) { addChars(chars); }
		charset(const charset& aset) { addCharset(aset); }

		
		// Convenience methods for adding and removing characters
		// in the set
		
		// Add a single character to the set
		charset& addChar(const char achar)
		{
			bits.set(achar);
			return *this;
		}

		// Add a range of characters to the set
		charset& addChars(const char* chars)
		{
			const char* s = chars;
			while (0 != *s)
				bits.set(*s++);
			return *this;
		}

		charset& addCharset(const charset& aset)
		{
			bits |= aset.bits;
			return *this;
		}

		// Methods for removing characters from the set
		charset& removeChar(const char achar)
		{
			bits.reset(achar);
			return *this;
		}
		
		charset& removeChars(const char* chars)
		{
			const char* s = chars;
			while (0 != *s)
				bits.reset(*s++);
			return *this;
		}
		
		charset& removeCharset(const charset& aset)
		{
			bits &= ~aset.bits;
			return *this;
		}
		
		// Convenience for adding characters and strings
		charset& operator+=(const char achar) { return addChar(achar); }
		charset& operator+=(const char* chars) { return addChars(chars); }
		charset& operator+=(const charset& aset) { return addCharset(aset); }
		
		// Creating a new set
		charset operator+(const char achar) const { charset result(*this); return result.addChar(achar); }
		charset operator+(const char* chars) const { charset result(*this); return result.addChars(chars); }
		charset operator+(const charset& aset) const { charset result(*this); return result.addCharset(aset); }
		
		// create operator- to remove a character from a set
		charset operator-(const char achar) const { charset result(*this); result.removeChar(achar); return result; }
		charset operator-(const char* chars) const { charset result(*this); result.removeChars(chars); return result; }
		charset operator-(const charset& aset) const { charset result(*this); result.removeCharset(aset); return result; }

		charset& operator-=(const char achar) { return removeChar(achar); }
		charset& operator-=(const char* chars) { return removeChars(chars); }

		
		// Checking for set membership
		bool contains(const uint8_t idx) const { return bits[idx];}
		bool operator [](const size_t idx) const { return bits[idx]; }
		bool operator ()(const size_t idx) const {return bits[idx];}
		
		// create operator^ to create a new set that is the union of two sets
		charset operator^(const charset& other) const { charset result(*this); result.bits |= other.bits; return result; }
		charset& operator^=(const charset& other) { bits |= other.bits; return *this; }
		
	};
}

// Some useful character sets
namespace pcore {
	static charset digitChars("0123456789");
	static charset hexChars("0123456789abcdefABCDEF");
	static charset octalChars("01234567");
	static charset binChars("01");

	static charset wspChars(" \r\n\t\f\v");		// a set of typical whitespace chars
	
	//	static charset alphaChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	//	static charset alphaNumChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");



}
