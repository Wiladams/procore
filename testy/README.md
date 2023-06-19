All these test cases are meant to be simple self contained executables
Compile with:
  cl -I ..\src fileaname.cpp


That's it.

Within the file, you can include whatever procore file headers you require

```C
#include "binops.h"

#include <cstdio>
#include <cstdlib>


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    char buff[9];
    pcore::tohex32(n, buff, 9);
    printf("%d = 0x%s\n", n, buff);
    return 0;
}
```