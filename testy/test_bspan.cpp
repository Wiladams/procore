


#include <cstdio>
#include <cstdlib>

#include "bspanutil.h"
#include "bspanprint.h"

using namespace pcore;

void test_trim()
{
    printf("==== test_trim ====\n");
    ByteSpan span1("  Hello, world!  ");
    ByteSpan span2 = chunk_trim(span1, wspChars);

    writeChunkBordered(span1);
    writeChunkBordered(span2);
}

void test_double()
{
    printf("==== test_double ====\n");
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
