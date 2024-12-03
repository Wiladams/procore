#include <cstdio>
#include <cstdlib>

#include "convspan.h"

void test_conv_hex()
{
    printf("==== test_conv_hex ====\n");
    bspan span1;
    bspan_init_from_cstr(&span1, "feedface55");

    uint64_t num;
    int err = bspan_conv_hex_to_u64(&span1, &num, nullptr);

    printf("HEX: 0x%llX\n", num);
}

void test_conv_double()
{
    printf("==== test_double ====\n");
    bspan spanNum1;
    bspan_init_from_cstr(&spanNum1, "123456e-3");

    double num1=0;
    bspan rest;
    int err = bspan_conv_to_double(&spanNum1, &num1, &rest);

    printf("num1 = %f\n", num1);
}

int main(int argc, char* argv[])
{
    test_conv_double();
    test_conv_hex();
}