
#include "bspanutil.h"

#include <cstdio>
#include <cstdlib>

using namespace pcore;

void test_trim()
{
    ByteSpan span1("  Hello, world!  ");
    ByteSpan span2 = chunk_trim(span1, wspChars);

    writeChunkBordered(span1);
    writeChunkBordered(span2);
}

void test_double()
{
    ByteSpan spanNum1("123.456");
    double num1 = toNumber(spanNum1);

    printf("num1 = %f\n", num1);
}

int main(int argc, char* argv[])
{
    ByteSpan span1("Hello, world!");

    printChunk(span1);

    test_double();
    test_trim();
}
