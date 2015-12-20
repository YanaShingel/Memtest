#include "sizes.h"

typedef unsigned long ul;
typedef unsigned long long ull;
typedef unsigned long volatile ulv;
typedef unsigned char volatile u8v;
typedef unsigned short volatile u16v;

struct test {
    char *name;
    int (*fp)(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count);
};

#if 0
union {
    unsigned char bytes[UL_LEN/8];
    ul val;
} mword8;
union {
    unsigned short u16s[UL_LEN/16];
    ul val;
} mword16;
#endif
