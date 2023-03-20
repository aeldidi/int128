#ifndef INT128_H
#define INT128_H
#include <stdint.h>

// TODO: define INT128_MAX, INT128_MIN, and UINT128_MAX

struct int128 {
	uint64_t low;
	uint64_t high;
};

typedef struct int128 int128;
typedef struct int128 uint128;

// Adds two int128s, wrapping on overflow.
static inline int128 int128_add(int128 lhs, int128 rhs)
{
	int128 result = {
		.low = lhs.low + rhs.low,
		.high = lhs.high + rhs.high,
	};

	if (result.low < lhs.low) {
		result.high += 1;
	}

	return result;
}

// Returns -x.
static inline int128 int128_neg(int128 x)
{
	return (int128){
		.low = ~x.low + 1,
		.high = ~x.high + (x.low == 0),
	};
}

// Subtracts two int128s, wrapping on underflow.
static inline int128 int128_sub(int128 lhs, int128 rhs)
{
	return int128_add(lhs, int128_neg(rhs));
}

// Multiply two unsigned 64 bit integers and get the 128 bit result.
static inline uint128 uint128_mul64(uint64_t lhs, uint64_t rhs)
{
	// Split the low 64 bits of lhs and rhs into its high and low 32 bits.
	uint64_t left_lo32 = lhs & UINT32_C(0xffffffff);
	uint64_t left_hi32 = lhs >> 32;
	uint64_t right_lo32 = rhs & UINT32_C(0xffffffff);
	uint64_t right_hi32 = rhs >> 32;

	// Compute each component of the product as the sum of multiple 32 bit
	// products.
	uint64_t product[4] = {
		left_lo32 * right_lo32,
		left_lo32 * right_hi32,
		left_hi32 * right_lo32,
		left_hi32 * right_hi32,
	};

	uint64_t carry =
		(((product[0] >> 32) + // high bits of low x low
		  (product[1] & UINT32_C(~0)) + // low bits of low x high
		  (product[2] & UINT32_C(~0))) // low bits of high x low
		 >> 32); // we want the high bits of that sum

	// Assemble the final product from these components, adding the carry
	// bits to the high 64 bits.
	return (uint128){
		.low = product[0] + (product[1] << 32) + (product[2] << 32),
		.high = product[3] + (product[1] >> 32) + (product[2] >> 32) +
			carry,
	};
}

// Multiply two signed 64 bit integers and get the 128 bit result.
static inline int128 int128_mul64(int64_t lhs, int64_t rhs)
{
	union {
		uint64_t u;
		int64_t i;
	} cvt;

	cvt.i = lhs;
	uint64_t a = cvt.u;

	cvt.i = rhs;
	uint64_t b = cvt.u;

	int128 result = uint128_mul64(a, b);

	if (lhs < 0) {
		result.high -= rhs;
	}

	if (rhs < 0) {
		result.high -= lhs;
	}

	return result;
}

// TODO: multiplication, division, modulo, bit operations.

#endif // INT128_H
