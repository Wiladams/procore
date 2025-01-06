

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
        printf("%c - %zd\n", c, bspan_size(&cspan));

        bspan_advance(&cspan, 1);
    }
    printf("\n");
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
    test_advance();
}
