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

	// Right Shift
	// For int128, this should be an arithmetic shift, meaning the sign bit
	// is preserved.
	a = INT128_C(-8);
	assert(int128_less(int128_shiftr(a, 1), INT128_C(0)));

	// Negation Sanity Check
	assert(int128_eq(int128_neg(INT128_MAX),
			 int128_add(INT128_MIN, INT128_C(1))));

	// Addition Sanity Check
	a = INT128_C(UINT64_MAX);
	b = int128_add(a, INT128_C(1));
	assert(int128_eq(b, (int128){ .high = 1 }));

	// Does addition wrap on overflow?
	b = int128_add(INT128_MAX, INT128_C(1));
	assert(int128_less(b, INT128_MAX));
	assert(int128_eq(b, INT128_MIN));

	// Subtraction Sanity Check
	b = int128_sub((int128){ .high = 1 }, INT128_C(1));
	assert(int128_eq(b, INT128_C(UINT64_MAX)));

	// Does subtraction wrap on underflow?
	b = int128_sub(INT128_MIN, INT128_C(1));
	assert(int128_eq(b, INT128_MAX));
	assert(int128_eq(b, INT128_MAX));

	// Multiplication Sanity Check
	a = (int128){ .high = 100000000 };
	b = int128_mul(a, INT128_C(2));
	assert(int128_eq(b, (int128){ .high = 200000000 }));

	b = int128_mul(INT128_MAX, INT128_C(0));
	assert(int128_eq(b, INT128_C(0)));

	// Does multiplication wrap on overflow?
	// We will try to compute INT128_MAX * -10 and check if it wrapped
	// around to 10.
	b = int128_mul(INT128_MAX, INT128_C(-10));
	assert(int128_eq(b, INT128_C(10)));

	// We'll also make sure that -INT128_MAX * -10 wraps to -10.
	b = int128_mul(int128_neg(INT128_MAX), INT128_C(-10));
	assert(int128_eq(b, INT128_C(-10)));

	// Make sure multiplying by -1 is equal to int128_neg.
	assert(int128_eq(int128_neg(INT128_MAX),
			 int128_mul(INT128_MAX, INT128_C(-1))));
	assert(int128_eq(int128_neg(INT128_MIN),
			 int128_mul(INT128_MIN, INT128_C(-1))));

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
	assert(uint128_eq(b, (uint128){ .high = 200000000 }));

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
