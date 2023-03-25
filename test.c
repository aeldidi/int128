#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#include "int128.h"

void int128_tests()
{
	// Left Shift
	int128 a = INT128_C(UINT64_C(1) << 63);
	int128 b = int128_shiftl(a, 1);
	assert(b.high == 1);

	a = (int128){ .high = 1 };
	b = int128_shiftr(a, 1);
	assert(b.low == (UINT64_C(1) << 63));

	// Addition Sanity Check
	a = INT128_C(UINT64_MAX);
	b = int128_add(a, INT128_C(1));
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
	assert(int128_eq(b, (int128){ .high = 100000000 }));
}

void uint128_tests()
{
	// Multiplication wraparound check
	// We will try to compute 1e38 * 10 and check if it wrapped around
	uint128 a = UINT128_C(1);
	for (int i = 0; i < 38; i += 1) {
		uint128 tmp = uint128_mul(a, UINT128_C(10));
		// This shouldn't overflow yet
		assert(uint128_greater(tmp, a));
		a = tmp;
	}

	// This will overflow
	uint128 b = uint128_mul(a, UINT128_C(10));

	// The result of the multiplication should be
	// 319435266158123073073250785136463577090, which has a prime
	// factorization of
	// 2 * 5 * 7 * 1187 * 25394857 * 1783251994049 * 84893729335057, so we
	// will represent it like that.

	uint128 tmp = uint128_mul64(2 * 5 * 7 * 1187, 25394857);
	tmp = uint128_mul(tmp, UINT128_C(1783251994049));
	tmp = uint128_mul(tmp, UINT128_C(84893729335057));

	// Sanity check: did this give us the right value?
	assert(tmp.high == 0xf050fe938943acc4);
	assert(tmp.low == 0x5f65568000000002);

	printf("our value = %llx%llx\n", b.high, b.low);
	printf("correct   = %llx%llx\n", tmp.high, tmp.low);

	assert(uint128_eq(b, tmp));
}

int main()
{
	int128_tests();
	uint128_tests();
}
