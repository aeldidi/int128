#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#include "int128.h"

void int128_tests()
{
	// Addition Sanity Check
	int128 a = INT128_C(UINT64_MAX);
	int128 b = int128_add(a, INT128_C(1));
	assert(int128_greater(b, a));
	assert(int128_less(b, int128_add(a, INT128_C(2))));
	assert(!int128_eq(b, a));

	// Subtraction Sanity Check
	b = int128_sub(b, INT128_C(1));
	assert(int128_eq(b, INT128_C(UINT64_MAX)));

	// Does addition wrap on overflow?
	b = int128_add(INT128_MAX, INT128_C(1));
	assert(int128_less(b, INT128_MAX));
	assert(int128_eq(b, INT128_MIN));

	// Does subtraction wrap on underflow?
	b = int128_sub(INT128_MIN, INT128_C(1));
	assert(int128_eq(b, INT128_MAX));
	assert(int128_eq(b, INT128_MAX));

	// Multiplication Sanity Check
	a = (int128){ .high = 100000000 };
	b = int128_mul(a, INT128_C(2));
	assert(b.high == 200000000);
	assert(b.low == 0);

	b = int128_mul(INT128_MAX, INT128_C(0));
	assert(int128_eq(b, INT128_C(0)));

	// TODO: does multiplication wrap on overflow?

	// Division Sanity Check
	a = (int128){ .high = 200000000 };
	b = int128_div(a, INT128_C(2));
	printf("%" PRId64 "%.20" PRIu64 " / 2 = %" PRId64 "%.20" PRIu64 "\n",
	       a.high, a.low, b.high, b.low);
	assert(b.high == 100000000);
}

int main()
{
	int128_tests();
}
