#include <stdio.h>

#include "bithacks.h"

void test_logicops()
{
    printf("==== test_logicops ====\n");

    int a, b;

    // Print the truth table
    printf("a b | NOT(a) AND OR XOR NAND NOR IMPLY EQUIV\n");
    for (a = 0; a <= 1; ++a) {
        for (b = 0; b <= 1; ++b) {
            printf("%d %d | %d      %d   %d  %d    %d    %d    %d     %d\n",
                a, b,
                _NOT(a)&1,
                _AND(a, b)&1,
                _OR(a, b)&1,
                _XOR(a, b)&1,
                _NAND(a, b)&1,
                _NOR(a, b)&1,
                _IMPLY(a, b)&1,
                _EQUIV(a, b)&1);
        }
    }
}


int main() 
{
    test_logicops();

    return 0;
}