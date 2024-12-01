#include <cstdio>
#include <cstdlib>


#include "charset.h"

using namespace pcore;

void printCharSet(CharSet &aset, const char *name = nullptr)
{
    if (nullptr != name)
        printf("**** %s ****\n", name);

    unsigned char c = 0;
    while (c < 255)
    {
        if (aset.contains(c))
            printf("%03d - %c\n", c, c);
        c++;
    }
}

void test_charset()
{
    printf("==== test_charset ====\n");
    printCharSet(digitChars, "digitChars");
    printCharSet(hexChars, "hexChars");
    printCharSet(octalChars, "octalChars");
    printCharSet(wspChars, "wspChars");
}

int main(int argc, char* argv[])
{
    test_charset();
}