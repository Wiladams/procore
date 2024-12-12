

#include <cstdio>
#include <cstdlib>

#include "bspan.h"



static void writeSpan(const bspan &bs)
{
    bspan s = bs;

    while (bspan_size(&s)>0)
    {
			printf("%c", *(bspan_data(&s)));
            bspan_advance(&s,1);
    }
}

static void printSpan(const bspan &bs)
{
    writeSpan(bs);
    printf("\n");
}

void test_advance()
{
    printf("== test_advance ==\n");
    bspan span1;
    bspan_init_from_cstr(&span1, "The Quick Brown Fox...");
    bspan cspan = span1;

    while (bspan_size(&cspan))
    {
        unsigned char c = *(bspan_data(&cspan));
        printf("%c - %d\n", c, bspan_size(&cspan));

        bspan_advance(&cspan, 1);
    }
    printf("\n");
}

void test_compare()
{
    bspan span1;
    bspan_init_from_cstr(&span1,"Beginning Middle End");

    bspan str1;
    bspan_init_from_cstr(&str1, "Beginning");
    bspan str2;
    bspan_init_from_cstr(&str2, "Beginning");

    int res1 = bspan_compare_span(&span1, &str1);
    int res2 = bspan_compare_span(&str1, &str2);
    int res3 = bspan_compare_span(&str1, &span1);

    printf("== Span Compare ==\n");
    printf("span1, str1: %d\n", res1);
    printf("str1, str2: %d\n", res2);
    printf("str1, span1: %d\n", res3);

}

void test_subspan()
{
    printf("== test_subspan == \n");

    bspan span1;
    bspan_init_from_cstr(&span1,"Beginning Middle End");

    bspan span2;
    bspan_subspan(&span1, 10,6, &span2);

    printSpan(span1);
    printSpan(span2);
}

int main(int argc, char* argv[])
{
    test_subspan();
//    test_compare();
    test_advance();
}
