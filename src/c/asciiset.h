#ifndef ASCIISET_H_INCLUDED
#define ASCIISET_H_INCLUDED



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

static int is_control(unsigned char) noexcept;
static int is_digit(unsigned char) noexcept;
static int is_extended(unsigned char) noexcept;
static int is_hex_digit(unsigned char) noexcept;
static int is_printable(unsigned char) noexcept;


static void asciiset_reset(asciiset *) noexcept;
static int asciiset_clone(asciiset *a, asciiset *b) noexcept;
static int asciiset_init_from_char(asciiset *, const unsigned char ) noexcept;
static int asciiset_init_from_cstr(asciiset *, const char *cstr) noexcept;

static int asciiset_add_char(asciiset *cs, const unsigned char c) noexcept;
static int asciiset_add_cstr(asciiset *, const char *cstr) noexcept;
static int asciiset_add_set(asciiset *, const asciiset *) noexcept;

static int asciiset_contains_char(const asciiset *cs, const unsigned char c) noexcept;

static int asciiset_remove_char(asciiset *cs, const unsigned char c) noexcept;
static int asciiset_remove_chars(asciiset *cs, const char *cstr) noexcept;
static int asciiset_remove_set(asciiset *cs, const asciiset *b) noexcept;


// Implementation
// convert a hex digit to a base 10 digit
// if conversion is successful, 0 is returned
static int is_control(unsigned char c) noexcept {return (c== 0x7f) || (c>=0 && c <= 0x1f);}
static int is_digit(unsigned char c) noexcept {return c>='0' && c <='9';}
static int is_extended(unsigned char c) noexcept {return c>=80 && c<=0xff;}
static int is_hex_digit(unsigned char c) noexcept {
    return (c>='a' && c<='f') ||
        (c>='A' && c<= 'F') ||
        (c>='0' && c<='9');
}
static int is_printable(unsigned char c) noexcept {return c>= 0x20 && c<=0x7e;}



static void asciiset_reset(asciiset *cs) noexcept
{
	// set all bytes to zero
	memset(cs->fBits, 0, ASCIISET_SIZE);
}

static int asciiset_clone(asciiset *a, asciiset *b) noexcept
{
	return 0;
}

static int asciiset_init_from_char(asciiset *cs, const unsigned char c) noexcept
{
	asciiset_reset(cs);
	return asciiset_add_char(cs, c);
}

static int asciiset_init_from_cstr(asciiset *cs, const char *cstr) noexcept
{
	asciiset_reset(cs);
	asciiset_add_cstr(cs, cstr);

	return 0;
}

static int asciiset_contains_char(const asciiset *cs, const unsigned char c) noexcept
{
	return bhak_get_bit_value(cs->fBits, ASCIISET_SIZE, c);
}



static int asciiset_remove_char(asciiset *cs, const unsigned char c) noexcept
{
	return bhak_remove_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_remove_chars(asciiset *cs, const char *cstr) noexcept
{
    char *str = (char *)cstr;
	while (*str)
	{
		bhak_remove_bit_value(cs->fBits, ASCIISET_SIZE, *str);
		str++;
	}

	return 0;
}


static int asciiset_remove_set(asciiset *cs, const asciiset *b) noexcept
{
    for (int i=0;i< ASCIISET_SIZE;i++)
		cs->fBits[i] &= ~b->fBits[i];
	
	return 0;
}


static int asciiset_add_char(asciiset *cs, const unsigned char c) noexcept
{
	return bhak_set_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_add_cstr(asciiset *cs, const char *cstr) noexcept
{
	char *str = (char *)cstr;
	while (*str)
	{
		bhak_set_bit_value(cs->fBits, ASCIISET_SIZE, *str);
		str++;
	}
    return 0;
}

static int asciiset_add_set(asciiset *a, const asciiset *b) noexcept
{
	for (int i=0;i< ASCIISET_SIZE;i++)
		a->fBits[i] |= b->fBits[i];
}



#ifdef __cplusplus
}
#endif

#endif