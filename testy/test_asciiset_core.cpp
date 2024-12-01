#include <cstdio>
#include <cstdlib>

#include "asciiset.h"

void printAsciiSet(asciiset &aset)
{
    unsigned char c = 0;
    while (c < 255)
    {
        if (asciiset_contains_char(&aset, c))
            printf("%03d - %c\n", c, c);
        c++;
    }
}

void test_asciiset()
{
    printf("==== test_asciiset ====\n");
    asciiset aset;
    asciiset_reset(&aset);

    asciiset_add_cstr(&aset, "abcDEF");
    printAsciiSet(aset);
}

int main(int argc, char* argv[])
{
    test_asciiset();
}
