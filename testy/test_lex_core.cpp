#include <cstdio>
#include <cstdlib>

#include "lexutil.h"

static void writeSpan(const bspan &bs)
{
    bspan s = bs;

    while (bspan_size(&s)>0)
    {
			printf("%c", *(bspan_data(&s)));
            bspan_advance(&s,1);
    }
}

void test_skip_until()
{
    asciiset wspset;
    asciiset_init_from_cstr(&wspset, " \t\r\n");

    bspan span1;
    bspan_init_from_cstr(&span1, "the quick and the dead");

    for (;;)
    {
        // skip leading whitespace
        bspan tok;
        bspan rest;
        lex_skip_leading_charset(&span1, &wspset, &span1);

        if (!bspan_is_valid(&span1))
            break;

        lex_skip_until_charset(&span1, &wspset, &tok, &span1);
        
        writeSpan(tok);
        printf("\n");
    }
}

// This is essentially a token
void test_skip_leading()
{
    printf("==== test_skip_until ====\n");
 
    bspan span1;
    bspan_init_from_cstr(&span1, "  the quick and the dead");

    bspan span2;
    asciiset wspset;
    asciiset_init_from_cstr(&wspset, " \t\r\n");
    lex_skip_leading_charset(&span1, &wspset, &span2);

    writeSpan(span1);
    printf("\n");
    writeSpan(span2);
}

int main(int argc, char* argv[])
{
    //test_skip_leading();
    test_skip_until();
}