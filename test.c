#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#include "int128.h"

// void int128_printhex(char *name, int128 x)
// {
// 	printf("%s = %016" PRIx64 " %016" PRIx64 "\n", name, x.high, x.low);
// }
//
// void uint128_printhex(char *name, uint128 x)
// {
// 	printf("%s = %016" PRIx64 " %016" PRIx64 "\n", name, x.high, x.low);
// }

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

	// Multiplication wraparound check
	// We will try to compute INT128_MAX * 10 and check if it wrapped
	// around to -10.
	b = int128_mul(INT128_MAX, INT128_C(10));
	assert(int128_eq(b, INT128_C(-10)));

	// Fun fact: INT128_MAX (170141183460469231731687303715884105727) is
	// prime :)

	// Division Sanity Check
	a = (int128){ .high = 200000000 };
	b = int128_div(a, INT128_C(2));
	assert(int128_eq(b, (int128){ .high = 100000000 }));
}

void uint128_tests()
{
	// Multiplication Sanity Check
	uint128 a = (uint128){ .high = 100000000 };
	uint128 b = uint128_mul(a, UINT128_C(2));
	assert(b.high == 200000000);
	assert(b.low == 0);

	// Multiplication wraparound check
	// We will try to compute 1e38 * 10 and check if it wrapped around
	a = UINT128_C(1);
	for (int i = 0; i < 38; i += 1) {
		uint128 tmp = uint128_mul(a, UINT128_C(10));
		// This shouldn't overflow yet
		assert(uint128_greater(tmp, a));
		a = tmp;
	}

	// This will overflow
	b = uint128_mul(a, UINT128_C(10));

	// The correct result after wrapping around is
	// 319435266158123073073250785136463577090, which is represented in hex
	// as
	uint128 correct = (uint128){
		.low = 0x5f65568000000000,
		.high = 0xf050fe938943acc4,
	};

	assert(uint128_eq(b, correct));
}

int main()
{
	uint128_tests();
	int128_tests();
}
