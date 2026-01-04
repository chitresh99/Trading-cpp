Well there is a new way to write hello world in c++ it seems 

```
#include <cstdio>
using namespace std;

int main()
{
    puts("hi");
    puts("this is");
    puts("me");
    return 0;
}

```

The first difference here we find is in the header file

```
#include <cstdio>
```

This is also a header file in c++ 

Also instead of cout we use 'puts' here because the cstdio contains puts so we can use it and the program will give the output as we wanted it to
