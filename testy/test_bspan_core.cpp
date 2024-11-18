

#include <cstdio>
#include <cstdlib>

#include "bspan.h"

using namespace pcore;

	static void writeSpan(const bspan_p chunk)
	{
		const unsigned char * s = bspan_begin(chunk);
        ptrdiff_t n = bspan_size(chunk);

        int i=0;
        while (i<n){
			printf("%c", *s);
            s++;
            i++;
        }
	}

static void printSpan(const bspan_p bs)
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
        unsigned char c = *cspan;
        printf("%c", c);

        bspan_advance(&cspan, 1, &cspan);
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

    struct bspan_t span1;
    bspan_init_from_cstr(&span1,"Beginning Middle End");

    struct bspan_t span2;
    bspan_subspan(&span1, 10,6, &span2);

    printSpan(&span1);
    printSpan(&span2);
}

int main(int argc, char* argv[])
{
    test_subspan();
    test_compare();
    test_advance();
}
