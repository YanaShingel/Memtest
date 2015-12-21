#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "types.h"
#include "sizes.h"

char mem_progress[] = "-\\|/";
#define PROGRESSLEN 4
#define PROGRESSOFTEN 2500
#define ONE 0x00000001L

#define ULONG_MAX (4294967295UL)
extern unsigned long get_timer_masked(void);

#define rand32() rand()

#if (ULONG_MAX == 4294967295UL)
    #define rand_ul() rand32()
    #define UL_ONEBITS 0xffffffff
    #define UL_LEN 32
    #define CHECKERBOARD1 0x55555555
    #define CHECKERBOARD2 0xaaaaaaaa
    #define UL_BYTE(x) ((x | x << 8 | x << 16 | x << 24))
#elif (ULONG_MAX == 18446744073709551615ULL)
    #define rand64() (((ul) rand32()) << 32 | ((ul) rand32()))
    #define rand_ul() rand64()
    #define UL_ONEBITS 0xffffffffffffffffUL
    #define UL_LEN 64
    #define CHECKERBOARD1 0x5555555555555555
    #define CHECKERBOARD2 0xaaaaaaaaaaaaaaaa
    #define UL_BYTE(x) (((ul)x | (ul)x<<8 | (ul)x<<16 | (ul)x<<24 | (ul)x<<32 | (ul)x<<40 | (ul)x<<48 | (ul)x<<56))
#else
    #error long on this platform is not 32 or 64 bits
#endif


void main() {

	size_t bufsize, halflen, count;
	bufsize = 3000;
	char *aligned = malloc(bufsize);
	long *bufa, *bufb;
	halflen = bufsize / 2;
	count = halflen / sizeof(long);
	bufa = (long *) aligned;
	bufb = (long *) ((size_t) aligned + halflen);

	test_random_value(bufa, bufb, count);
	test_xor_comparison(bufa, bufb, count);
	test_sub_comparison(bufa, bufb, count);
	test_mul_comparison(bufa, bufb, count);
	test_div_comparison(bufa, bufb, count);
	test_or_comparison(bufa, bufb, count);
	test_and_comparison(bufa, bufb, count);
	test_seqinc_comparison(bufa, bufb, count);
	test_solidbits_comparison(bufa, bufb, count);
	test_checkerboard_comparison(bufa, bufb, count);
	test_blockseq_comparison(bufa, bufb, count);
	test_walkbits0_comparison(bufa, bufb, count);
	test_walkbits1_comparison(bufa, bufb, count);
	test_bitspread_comparison(bufa, bufb, count);
	test_bitflip_comparison(bufa, bufb, count);
}

int compare_regions(long *bufa, long *bufb, size_t count) {
    int r = 0;
    size_t i;
    long *p1 = bufa;
    long *p2 = bufb;

    for (i = 0; i < count; i++, p1++, p2++) {
        if (*p1 != *p2) {
                printf(
                        "FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n",
                        (long) *p1, (long) *p2, (long) (i * sizeof(long)));
            /* printf("Skipping to next test..."); */
            r = -1;
        }
    }
    return r;
}

int test_stuck_address(long *bufa, size_t count) {
    long *p1 = bufa;
    unsigned int j;
    size_t i;

    printf("           ");
    for (j = 0; j < 16; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (long *) bufa;
        printf("setting %3u", j);
        for (i = 0; i < count; i++) {
            *p1 = ((j + i) % 2) == 0 ? (long) p1 : ~((long) p1);
            *p1 = p1 + 1;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        p1 = (long *) bufa;
        for (i = 0; i < count; i++, p1++) {
            if (*p1 != (((j + i) % 2) == 0 ? (long) p1 : ~((long) p1))) {
                printf("Skipping to next test...\n");
                //fflush(stdout);
                return -1;
            }
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_random_value(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    long j = 0;
    size_t i;

    printf(" ");
    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = rand_ul();
        if (!(i % PROGRESSOFTEN)) {
            printf("\b");
            printf("%c",mem_progress[++j % PROGRESSLEN]);
        }
    }
    printf("\b \b");
    return compare_regions(bufa, bufb, count);
}

int test_xor_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ ^= q;
        *p2++ ^= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_sub_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ -= q;
        *p2++ -= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_mul_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ *= q;
        *p2++ *= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_div_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        if (!q) {
            q++;
        }
        *p1++ /= q;
        *p2++ /= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_or_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ |= q;
        *p2++ |= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_and_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ &= q;
        *p2++ &= q;
    }
    return compare_regions(bufa, bufb, count);
}

int test_seqinc_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    size_t i;
    long q = rand_ul();

    for (i = 0; i < count; i++) {
        *p1++ = *p2++ = (i + q);
    }
    return compare_regions(bufa, bufb, count);
}

int test_solidbits_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    unsigned int j;
    long q;
    size_t i;

    printf("           ");
    for (j = 0; j < 64; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? UL_ONEBITS : 0;
        printf("setting %3u", j);
        p1 = (long *) bufa;
        p2 = (long *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_checkerboard_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    unsigned int j;
    long q;
    size_t i;

    printf("           ");
    for (j = 0; j < 64; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
        printf("setting %3u", j);
        p1 = (long *) bufa;
        p2 = (long *) bufb;
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_blockseq_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    for (j = 0; j < 256; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (long *) bufa;
        p2 = (long *) bufb;
        printf("setting %3u", j);
        for (i = 0; i < count; i++) {
            *p1++ = *p2++ = (long) UL_BYTE(j);
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_walkbits0_comparison(long *bufa, long *bufb, size_t count) {
    long *p1 = bufa;
    long *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    for (j = 0; j < UL_LEN  * 2; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (long *) bufa;
        p2 = (long *) bufb;
        printf("setting %3u", j);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN ) { /* Walk it up. */
                *p1++ = *p2++ = ONE << j;
            } else { /* Walk it back down. */
                *p1++ = *p2++ = ONE << (UL_LEN  * 2 - j - 1);
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_walkbits1_comparison(long *bufa, long *bufb, size_t count) {
	long *p1 = bufa;
	long *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    for (j = 0; j < UL_LEN * 2; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (long *) bufa;
        p2 = (long *) bufb;
        printf("setting %3u", j);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << j);
            } else { /* Walk it back down. */
                *p1++ = *p2++ = UL_ONEBITS ^ (ONE << (UL_LEN * 2 - j - 1));
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_bitspread_comparison(long *bufa, long *bufb, size_t count) {
	long *p1 = bufa;
	long *p2 = bufb;
    unsigned int j;
    size_t i;

    printf("           ");
    for (j = 0; j < UL_LEN * 2; j++) {
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        p1 = (long *) bufa;
        p2 = (long *) bufb;
        printf("setting %3u", j);
        for (i = 0; i < count; i++) {
            if (j < UL_LEN) { /* Walk it up. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (ONE << j) | (ONE << (j + 2))
                    : UL_ONEBITS ^ ((ONE << j)
                                    | (ONE << (j + 2)));
            } else { /* Walk it back down. */
                *p1++ = *p2++ = (i % 2 == 0)
                    ? (ONE << (UL_LEN * 2 - 1 - j)) | (ONE << (UL_LEN * 2 + 1 - j))
                    : UL_ONEBITS ^ (ONE << (UL_LEN * 2 - 1 - j)
                                    | (ONE << (UL_LEN * 2 + 1 - j)));
            }
        }
        printf("\b\b\b\b\b\b\b\b\b\b\b");
        printf("testing %3u", j);
        if (compare_regions(bufa, bufb, count)) {
            return -1;
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}

int test_bitflip_comparison(long *bufa, long *bufb, size_t count) {
	long *p1 = bufa;
	long *p2 = bufb;
    unsigned int j, k;
    long q;
    size_t i;

    printf("           ");
    for (k = 0; k < UL_LEN; k++) {
        q = ONE << k;
        for (j = 0; j < 8; j++) {
            printf("\b\b\b\b\b\b\b\b\b\b\b");
            q = ~q;
            printf("setting %3u", k * 8 + j);
            p1 = (long *) bufa;
            p2 = (long *) bufb;
            for (i = 0; i < count; i++) {
                *p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
            }
            printf("\b\b\b\b\b\b\b\b\b\b\b");
            printf("testing %3u", k * 8 + j);
            if (compare_regions(bufa, bufb, count)) {
                return -1;
            }
        }
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");
    return 0;
}
