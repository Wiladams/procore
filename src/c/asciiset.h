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
//
// In addition, we collect some ascii character categorizations here as well
// Why bother when we have <ctype>?  No particular reason really, other than
// completeness.  These can easily be ignored.
//
// ASCII CONTROL	0x00 : 0x15
// ASCII PRINTABLE 	0x16 : 0x7f
// ASCII EXTENDED	0x80 : 0xff

#define ASCIISET_SIZE 32

struct asciiset_t {
	unsigned char fBits[ASCIISET_SIZE];
};
typedef struct asciiset_t asciiset;

static int is_control(unsigned char) PC_NOEXCEPT_C;
static int is_digit(unsigned char) PC_NOEXCEPT_C;
static int is_extended(unsigned char) PC_NOEXCEPT_C;
static int is_hex_digit(unsigned char) PC_NOEXCEPT_C;
static int is_printable(unsigned char) PC_NOEXCEPT_C;


static void asciiset_reset(asciiset *) PC_NOEXCEPT_C;
static int asciiset_clone(asciiset *a, asciiset *b) PC_NOEXCEPT_C;
static int asciiset_init_from_char(asciiset *, const unsigned char ) PC_NOEXCEPT_C;
static int asciiset_init_from_cstr(asciiset *, const char *cstr) PC_NOEXCEPT_C;

static int asciiset_add_char(asciiset *cs, const unsigned char c) PC_NOEXCEPT_C;
static int asciiset_add_cstr(asciiset *, const char *cstr) PC_NOEXCEPT_C;
static int asciiset_add_set(asciiset *, const asciiset *) PC_NOEXCEPT_C;

static int asciiset_contains_char(const asciiset *cs, const unsigned char c) PC_NOEXCEPT_C;

static int asciiset_remove_char(asciiset *cs, const unsigned char c) PC_NOEXCEPT_C;
static int asciiset_remove_chars(asciiset *cs, const char *cstr) PC_NOEXCEPT_C;
static int asciiset_remove_set(asciiset *cs, const asciiset *b) PC_NOEXCEPT_C;


// Implementation
// convert a hex digit to a base 10 digit
// if conversion is successful, 0 is returned
static int is_control(unsigned char c) PC_NOEXCEPT_C {return (c== 0x7f) || (c>=0 && c <= 0x1f);}
static int is_digit(unsigned char c) PC_NOEXCEPT_C {return c>='0' && c <='9';}
static int is_extended(unsigned char c) PC_NOEXCEPT_C {return c>=80 && c<=0xff;}
static int is_hex_digit(unsigned char c) PC_NOEXCEPT_C {
    return (c>='a' && c<='f') ||
        (c>='A' && c<= 'F') ||
        (c>='0' && c<='9');
}
static int is_printable(unsigned char c) PC_NOEXCEPT_C {return c>= 0x20 && c<=0x7e;}



static void asciiset_reset(asciiset *cs) PC_NOEXCEPT_C
{
	// set all bytes to zero
	memset(cs->fBits, 0, ASCIISET_SIZE);
}

static int asciiset_clone(asciiset *a, asciiset *b) PC_NOEXCEPT_C
{
	return 0;
}

static int asciiset_init_from_char(asciiset *cs, const unsigned char c) PC_NOEXCEPT_C
{
	asciiset_reset(cs);
	return asciiset_add_char(cs, c);
}

static int asciiset_init_from_cstr(asciiset *cs, const char *cstr) PC_NOEXCEPT_C
{
	asciiset_reset(cs);
	asciiset_add_cstr(cs, cstr);

	return 0;
}

static int asciiset_contains_char(const asciiset *cs, const unsigned char c) PC_NOEXCEPT_C
{
	return bhak_get_bit_value(cs->fBits, ASCIISET_SIZE, c);
}



static int asciiset_remove_char(asciiset *cs, const unsigned char c) PC_NOEXCEPT_C
{
	return bhak_remove_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_remove_chars(asciiset *cs, const char *cstr) PC_NOEXCEPT_C
{
    char *str = (char *)cstr;
	while (*str)
	{
		bhak_remove_bit_value(cs->fBits, ASCIISET_SIZE, *str);
		str++;
	}

	return 0;
}


static int asciiset_remove_set(asciiset *cs, const asciiset *b) PC_NOEXCEPT_C
{
    for (int i=0;i< ASCIISET_SIZE;i++)
		cs->fBits[i] &= ~b->fBits[i];
	
	return 0;
}


static int asciiset_add_char(asciiset *cs, const unsigned char c) PC_NOEXCEPT_C
{
	return bhak_set_bit_value(cs->fBits, ASCIISET_SIZE, c);
}

static int asciiset_add_cstr(asciiset *cs, const char *cstr) PC_NOEXCEPT_C
{
	char *str = (char *)cstr;
	while (*str)
	{
		bhak_set_bit_value(cs->fBits, ASCIISET_SIZE, *str);
		str++;
	}
    return 0;
}

static int asciiset_add_set(asciiset *a, const asciiset *b) PC_NOEXCEPT_C
{
	for (int i=0;i< ASCIISET_SIZE;i++)
		a->fBits[i] |= b->fBits[i];
}


static asciiset * wspcharset() PC_NOEXCEPT_C
{
    static asciiset chars;
    static bool initialized=false;

    if (!initialized){
        asciiset_init_from_cstr(&chars, " \r\n\t");
        initialized = true;
    }

    return &chars;
}

#ifdef __cplusplus
}
#endif

#endif