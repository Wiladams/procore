#include <cstdio>
#include <cstdlib>

#include "lexutil.h"
#include "bspanprint.h"



void test_front_token()
{
    printf("==== test_front_token ====\n");

    asciiset wspset;
    asciiset_init_from_cstr(&wspset, " \t\r\n");

    bspan span1;
    bspan_init_from_cstr(&span1, "  they were the best of times");

    bspan rest;
    bspan_weak_assign(&rest, &span1);

    for (;;)
    {
        bspan tok;

        lex_front_token(&rest, &wspset, &wspset, &tok, &rest);

        if (!bspan_is_valid(&tok))
            break;

        printSpan(tok);
        printSpan(rest);
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

void test_skip_trailing()
{
    printf("==== test_skip_trailing ====\n");

    bspan span1;
    bspan_init_from_cstr(&span1, "this has a long tail    \r\n");

    bspan span2;
    asciiset wspset;
    asciiset_init_from_cstr(&wspset, " \t\r\n");

    lex_skip_trailing_charset(&span1, &wspset, &span2);

    printf("|");
    writeSpan(span2);
    printf("|\n");

}

int main(int argc, char* argv[])
{
    //test_skip_leading();
    test_skip_trailing();
    //test_skip_until();
    test_front_token();
}