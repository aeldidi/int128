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

	// Left shift of more than 63 bits?
	assert(int128_eq(int128_shiftl(INT128_C(1), 64),
			 (int128){ .high = 1 }));

	// Right Shift
	// For int128, this should be an arithmetic shift, meaning the sign bit
	// is preserved.
	a = INT128_C(-8);
	assert(int128_less(int128_shiftr(a, 1), INT128_C(0)));

	// Right shift of more than 63 bits?
	assert(int128_eq(int128_shiftr((int128){ .high = 1 }, 64),
			 INT128_C(1)));
	assert(int128_eq(int128_shiftr((int128){ .high = 0xfffffffffffffffe,
						 .low = UINT64_MAX },
				       64),
			 INT128_C(-2)));
	assert(int128_eq(int128_shiftr((int128){ .high = 0xbfffffffffffffff,
						 .low = UINT64_MAX },
				       126),
			 INT128_C(-2)));

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
	assert(int128_eq(int128_neg(INT128_MIN),
			 int128_mul(INT128_C(-1), INT128_MIN)));

	// Fun fact: INT128_MAX (170141183460469231731687303715884105727) is
	// prime :)

	// Division Sanity Check
	a = (int128){ .high = 200000000 };
	b = int128_div(a, INT128_C(2));
	assert(int128_eq(b, (int128){ .high = 100000000 }));

	// INT128_MIN divided by anything is 0, since INT128_MIN is less than
	// every other number...
	assert(int128_eq(int128_div(INT128_MIN, INT128_C(-123456789)),
			 INT128_C(0)));

	// Except for when dividing INT128_MIN by INT128_MIN, which should be
	// 1.
	assert(int128_eq(int128_div(INT128_MIN, INT128_MIN), INT128_C(1)));

	// Check to make sure the sign is preserved properly:
	// - / - = +
	// - / + = -
	// + / - = -
	// + / + = +

	a = INT128_C(1);
	b = INT128_C(-1);
	assert(int128_eq(int128_div(b, b), a));
	assert(int128_eq(int128_div(b, a), b));
	assert(int128_eq(int128_div(a, b), b));
	assert(int128_eq(int128_div(a, a), a));

	// And of course anytime the denominator is greater than the numerator
	// the result is also 0.
	assert(int128_eq(int128_div(INT128_C(1), INT128_C(2)), INT128_C(0)));

	// Remainder Sanity Check
	// 85070591730234615865843651857942052863 % 1234567890 = 1122498603
	a = int128_div(int128_sub(INT128_MAX, INT128_C(1)), INT128_C(2));
	b = INT128_C(1234567890);
	assert(int128_eq(int128_rem(a, b), INT128_C(1122498603)));

	a = int128_div(int128_sub(INT128_MAX, INT128_C(1)), INT128_C(2));
	b = INT128_C(-1234567890);

	// The remainder operation has the same sign as the left hand side.
	assert(int128_eq(int128_rem(a, b), INT128_C(1122498603)));
	a = int128_neg(a);
	assert(int128_eq(int128_rem(a, b), INT128_C(-1122498603)));

	// The remainder of anything divided by INT128_MIN is the left hand
	// side...
	assert(int128_eq(int128_rem(INT128_C(-1), INT128_MIN), INT128_C(-1)));

	// Except for when the left hand side is INT128_MIN, in which case the
	// remainder is 0.
	assert(int128_eq(int128_rem(INT128_MIN, INT128_MIN), INT128_C(0)));

	// This applies to all numbers.
	assert(int128_eq(int128_rem(INT128_C(1), INT128_C(1)), INT128_C(0)));

	// When both the numerator and denominator are positive, if the
	// denominator is greater than the numerator the result is just the
	// numerator.
	assert(int128_eq(int128_rem(INT128_C(1), INT128_C(2)), INT128_C(1)));
}

void uint128_tests()
{
	// For most functions, the same logic is used for positive int128 and
	// uint128, so we only need to test things which we can't use int128 to
	// test, like large multiplication overflow.

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

	// Right Shift Sanity Check
	// For unsigned integers, the most significant bit should not be
	// preserved.
	a = (uint128){ .high = UINT64_C(1) << 63 };
	assert(uint128_eq(uint128_shiftr(a, 1),
			  (uint128){ .high = UINT64_C(1) << 62 }));

	// uint128_less when high bits are equal?
	assert(uint128_less(UINT128_C(2), UINT128_C(5)));
}

int main()
{
	uint128_tests();
	int128_tests();
}
