#ifndef INT128_H
#define INT128_H
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

#define INT128_MAX \
	((int128){ .low = UINT64_MAX, .high = UINT64_C(0x7fffffffffffffff) })
#define INT128_MIN ((int128){ .low = 0, .high = UINT64_C(0x8000000000000000) })
#define UINT128_MAX ((uint128){ .low = UINT64_MAX, .high = UINT64_MAX })

#define UINT128_C(x) ((uint128){ .low = x })
#define INT128_C(x)                                             \
	((x < 0) ? (int128){ .low = x, .high = UINT64_C(~0) } : \
		   (int128){ .low = x })

struct int128 {
	uint64_t low;
	uint64_t high;
};

struct uint128 {
	uint64_t low;
	uint64_t high;
};

typedef struct int128 int128;
typedef struct uint128 uint128;

// Returns lhs < rhs.
static inline bool uint128_less(uint128 lhs, uint128 rhs)
{
	if (lhs.high == rhs.high) {
		return lhs.low < rhs.low;
	}

	return lhs.high < rhs.high;
}

// Returns lhs > rhs.
static inline bool uint128_greater(uint128 lhs, uint128 rhs)
{
	if (lhs.high == rhs.high) {
		return lhs.low > rhs.low;
	}

	return lhs.high > rhs.high;
}

// Returns lhs < rhs.
static inline bool int128_less(int128 lhs, int128 rhs)
{
	union {
		uint64_t u;
		int64_t i;
	} cvt;
	union {
		uint64_t u;
		int64_t i;
	} cvt2;

	cvt.u = lhs.high;
	cvt2.u = rhs.high;

	if (cvt.i == cvt2.i) {
		cvt.u = lhs.low;
		cvt2.u = rhs.low;
		return cvt.i < cvt2.i;
	}

	return cvt.i < cvt2.i;
}

// Returns lhs > rhs.
static inline bool int128_greater(int128 lhs, int128 rhs)
{
	union {
		uint64_t u;
		int64_t i;
	} cvt;
	union {
		uint64_t u;
		int64_t i;
	} cvt2;

	cvt.u = lhs.high;
	cvt2.u = rhs.high;

	if (cvt.i == cvt2.i) {
		cvt.u = lhs.low;
		cvt2.u = rhs.low;
		return cvt.i > cvt2.i;
	}

	return cvt.i > cvt2.i;
}

// Returns lhs == rhs.
static inline bool int128_eq(int128 lhs, int128 rhs)
{
	return lhs.low == rhs.low && lhs.high == rhs.high;
}

// Returns lhs == rhs.
static inline bool uint128_eq(uint128 lhs, uint128 rhs)
{
	return lhs.low == rhs.low && lhs.high == rhs.high;
}

// Returns lhs <= rhs.
static inline bool uint128_less_eq(uint128 lhs, uint128 rhs)
{
	return uint128_less(lhs, rhs) || uint128_eq(lhs, rhs);
}

// Returns lhs >= rhs.
static inline bool uint128_greater_eq(uint128 lhs, uint128 rhs)
{
	return uint128_greater(lhs, rhs) || uint128_eq(lhs, rhs);
}

// Returns lhs >= rhs.
static inline bool int128_greater_eq(int128 lhs, int128 rhs)
{
	return int128_greater(lhs, rhs) || int128_eq(lhs, rhs);
}

// Returns lhs <= rhs.
static inline bool int128_less_eq(int128 lhs, int128 rhs)
{
	return int128_less(lhs, rhs) || int128_eq(lhs, rhs);
}

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

// Adds two uint128s, wrapping on overflow.
static inline uint128 uint128_add(uint128 lhs, uint128 rhs)
{
	int128 a = { lhs.low, lhs.high };
	int128 b = { rhs.low, rhs.high };
	int128 tmp = int128_add(a, b);
	return (uint128){
		.low = tmp.low,
		.high = tmp.high,
	};
}

// Returns -x.
static inline int128 int128_neg(int128 x)
{
	return (int128){
		.low = ~x.low + 1,
		.high = ~x.high + (x.low == 0),
	};
}

// Returns ~x.
static inline int128 int128_compl(int128 x)
{
	return (int128){
		.low = ~x.low,
		.high = ~x.high,
	};
}

// Returns ~x.
static inline uint128 uint128_compl(uint128 x)
{
	return (uint128){
		.low = ~x.low,
		.high = ~x.high,
	};
}

// Returns lhs | rhs.
static inline int128 int128_or(int128 lhs, int128 rhs)
{
	return (int128){
		.low = lhs.low | rhs.low,
		.high = lhs.high | rhs.high,
	};
}

// Returns lhs & rhs.
static inline int128 int128_and(int128 lhs, int128 rhs)
{
	return (int128){
		.low = lhs.low & rhs.low,
		.high = lhs.high & rhs.high,
	};
}

// Returns lhs | rhs.
static inline uint128 uint128_or(uint128 lhs, uint128 rhs)
{
	return (uint128){
		.low = lhs.low | rhs.low,
		.high = lhs.high | rhs.high,
	};
}

// Returns lhs & rhs.
static inline uint128 uint128_and(uint128 lhs, uint128 rhs)
{
	return (uint128){
		.low = lhs.low & rhs.low,
		.high = lhs.high & rhs.high,
	};
}

// Returns lhs ^ rhs.
static inline int128 int128_xor(int128 lhs, int128 rhs)
{
	return (int128){
		.low = lhs.low ^ rhs.low,
		.high = lhs.high ^ rhs.high,
	};
}

// Returns lhs ^ rhs.
static inline uint128 uint128_xor(uint128 lhs, uint128 rhs)
{
	return (uint128){
		.low = lhs.low ^ rhs.low,
		.high = lhs.high ^ rhs.high,
	};
}

// Returns lhs << rhs.
static inline int128 int128_shiftl(int128 lhs, int rhs)
{
	if (rhs >= 64) {
		return (int128){ .low = 0, .high = lhs.low << (rhs - 64) };
	}

	return (int128){
		.low = lhs.low << rhs,
		.high = (lhs.high << rhs) | (lhs.low >> (64 - rhs)),
	};
}

// Returns lhs << rhs.
static inline uint128 uint128_shiftl(uint128 lhs, int rhs)
{
	int128 a = { .low = lhs.low, .high = lhs.high };
	int128 result = int128_shiftl(a, rhs);
	return (uint128){ .low = result.low, .high = result.high };
}

// Returns lhs >> rhs.
static inline uint128 uint128_shiftr(uint128 lhs, int rhs)
{
	if (rhs >= 64) {
		return (uint128){ .low = lhs.high >> (rhs - 64), .high = 0 };
	}

	return (uint128){
		.low = (lhs.low >> rhs) | (lhs.high << (64 - rhs)),
		.high = (lhs.high >> rhs),
	};
}

// Returns lhs >> rhs.
// On signed int128 this is an arithmetic shift, so the sign bit is used for
// shifted in bits.
static inline int128 int128_shiftr(int128 lhs, int rhs)
{
	if (int128_greater(lhs, INT128_C(0))) {
		uint128 a = { .low = lhs.low, .high = lhs.high };
		uint128 result = uint128_shiftr(a, rhs);
		return (int128){ .low = result.low, .high = result.high };
	}

	if (rhs >= 64) {
		return (int128){
			.low = (lhs.high >> (rhs - 64)) | (UINT64_C(~0) << rhs),
			.high = UINT64_C(~0),
		};
	}

	return (int128){
		.low = (lhs.low >> rhs) | (lhs.high << (64 - rhs)),
		.high = (lhs.high >> rhs) | (UINT64_C(~0) << (64 - rhs)),
	};
}

// Subtracts two int128s, wrapping on underflow.
static inline int128 int128_sub(int128 lhs, int128 rhs)
{
	return int128_add(lhs, int128_neg(rhs));
}

// Subtracts two uint128s, wrapping on underflow.
static inline uint128 uint128_sub(uint128 lhs, uint128 rhs)
{
	uint128 result = { lhs.low - rhs.low };
	uint64_t carry = (((result.low & rhs.low) & 1) + (rhs.low >> 1) +
			  (result.low >> 1)) >>
			 63;

	result.high = lhs.high - (rhs.high + carry);
	return result;
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

	uint128 tmp = uint128_mul64(a, b);
	int128 result = {
		.low = tmp.low,
		.high = tmp.high,
	};

	if (lhs < 0) {
		result.high -= rhs;
	}

	if (rhs < 0) {
		result.high -= lhs;
	}

	return result;
}

// Returns lhs * rhs.
static inline uint128 uint128_mul(uint128 lhs, uint128 rhs)
{
	uint128 result = uint128_mul64(lhs.low, rhs.low);
	result.high += (lhs.high * rhs.low) + (lhs.low * rhs.high);
	return result;
}

// Returns lhs * rhs.
static inline int128 int128_mul(int128 lhs, int128 rhs)
{
	bool result_negative = false;
	if (int128_less(lhs, INT128_C(0))) {
		result_negative = !result_negative;
	}

	if (int128_less(rhs, INT128_C(0))) {
		result_negative = !result_negative;
	}

	lhs = int128_neg(lhs);
	rhs = int128_neg(rhs);

	uint128 a = { .low = lhs.low, .high = lhs.high };
	uint128 b = { .low = rhs.low, .high = rhs.high };
	uint128 tmp = uint128_mul(a, b);
	int128 result = { .low = tmp.low, .high = tmp.high };
	if (result_negative) {
		result = int128_neg(result);
	}

	return result;
}

// Returns lhs / rhs.
static inline uint128 uint128_div(uint128 lhs, uint128 rhs)
{
	// Based on the algorithm described here:
	// https://stackoverflow.com/questions/5386377/division-without-using
	if (uint128_greater(rhs, lhs)) {
		return UINT128_C(0);
	}

	if (uint128_eq(lhs, rhs)) {
		return UINT128_C(1);
	}

	uint128 current = UINT128_C(1);
	uint128 result = UINT128_C(0);
	uint128 divisor = rhs;
	while (uint128_less_eq(divisor, lhs)) {
		divisor = uint128_shiftl(divisor, 1);
		current = uint128_shiftl(current, 1);
	}

	divisor = uint128_shiftr(divisor, 1);
	current = uint128_shiftr(current, 1);

	while (!uint128_eq(current, UINT128_C(0))) {
		if (uint128_greater_eq(lhs, divisor)) {
			lhs = uint128_sub(lhs, divisor);
			result = uint128_or(result, current);
		}

		current = uint128_shiftr(current, 1);
		divisor = uint128_shiftr(divisor, 1);
	}

	return result;
}

// Returns lhs % rhs.
static inline uint128 uint128_mod(uint128 lhs, uint128 rhs)
{
	// Based on the algorithm described here:
	// https://stackoverflow.com/questions/5386377/division-without-using
	if (uint128_greater(rhs, lhs)) {
		return lhs;
	}

	if (uint128_eq(lhs, rhs)) {
		return UINT128_C(0);
	}

	uint128 current = UINT128_C(1);
	uint128 result = UINT128_C(0);
	uint128 divisor = rhs;
	while (uint128_less_eq(divisor, lhs)) {
		divisor = uint128_shiftl(divisor, 1);
		current = uint128_shiftl(current, 1);
	}

	divisor = uint128_shiftr(divisor, 1);
	current = uint128_shiftr(current, 1);

	while (!uint128_eq(current, UINT128_C(0))) {
		if (uint128_greater_eq(lhs, divisor)) {
			lhs = uint128_sub(lhs, divisor);
			result = uint128_or(result, current);
		}

		current = uint128_shiftr(current, 1);
		divisor = uint128_shiftr(divisor, 1);
	}

	return lhs;
}

// Returns lhs / rhs.
static inline int128 int128_div(int128 lhs, int128 rhs)
{
	// signed integers aren't symmetric. INT128_MIN == (-INT128_MAX) - 1
	if (int128_eq(lhs, INT128_MIN)) {
		return INT128_C(0);
	}

	bool result_negative = false;
	if (int128_less(lhs, INT128_C(0))) {
		result_negative = !result_negative;
		lhs = int128_neg(lhs);
	}

	// TODO: negative demonimator

	uint128 a = { .low = lhs.low, .high = lhs.high };
	uint128 b = { .low = rhs.low, .high = rhs.low };
	uint128 c = uint128_div(a, b);
	int128 result = { .low = c.low, .high = c.high };

	if (result_negative) {
		return int128_neg(result);
	}

	return result;
}

// Returns lhs % rhs.
static inline int128 int128_mod(int128 lhs, int128 rhs)
{
	// signed integers aren't symmetric. INT128_MIN == (-INT128_MAX) - 1
	if (int128_eq(lhs, INT128_MIN)) {
		return INT128_C(0);
	}

	bool result_negative = false;
	if (int128_less(lhs, INT128_C(0))) {
		result_negative = !result_negative;
		lhs = int128_neg(lhs);
	}

	// TODO: negative demonimator

	uint128 a = { .low = lhs.low, .high = lhs.high };
	uint128 b = { .low = rhs.low, .high = rhs.low };
	uint128 c = uint128_mod(a, b);
	int128 result = { .low = c.low, .high = c.high };

	if (result_negative) {
		return int128_neg(result);
	}

	return result;
}

#endif // INT128_H
