#include <inttypes.h>

// This prelude implements enough of C0 that any C0 code can be compiled with a
// C compiler, as long as this file is included before any C0 code.
//
// Features required for this to work:
// - IEEE-754 floating point representation
// - Type-punning with unions which is not a trap representation. Accessing the
//   union object using the member which was not stored last should reinterpret
//   the bits of the first object as the second (This is how GCC does it).
// - Integers must be represented as 2s complement (This is how most compilers
//   do it).
//

typedef void *ptr;
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

struct __internal_c0_int128 {
	u64 low;
	u64 high;
};

typedef struct __internal_c0_int128 i128;
typedef struct __internal_c0_int128 u128;

#if !defined(__STDC_IEC_559__)
#error "IEEE-754 floating point support is needed!"
#endif

typedef uint16_t f16;
typedef float f32;
typedef long double f64;

#if (-1 & 3) != 3
#error "2s complement representation for integers is needed!"
#endif

// int128 support library
// because both are 2s complement, sign doesn't matter.

i128 __c0_i128_add(i128 lhs, i128 rhs)
{
	i128 result = {
		.low = lhs.low + rhs.low,
		.high = lhs.high + rhs.high,
	};

	if (result.low < lhs.low) {
		result.high += 1;
	}

	return result;
}

i128 __c0_i128_neg(i128 x)
{
	return (i128){
		.low = ~x.low + 1,
		.high = ~x.high + (x.low == 0),
	};
}

i128 __c0_i218_sub(i128 lhs, i128 rhs)
{
	return __c0_i128_add(lhs, __c0_i128_neg(rhs));
}

// Multiply two unsigned 64 bit integers and get the 128 bit result.
u128 __c0_u64_mul(u64 lhs, u64 rhs)
{
	// Split the low 64 bits of lhs and rhs into its high and low 32 bits.
	u64 left_lo32 = (u32)(lhs & UINT32_C(0xffffffff));
	u64 left_hi32 = (u32)(lhs >> 32);
	u64 right_lo32 = (u32)(rhs & UINT32_C(0xffffffff));
	u64 right_hi32 = (u32)(rhs >> 32);

	// Compute each component of the product as the sum of multiple 32 bit
	// products.
	u64 product[4] = {
		left_lo32 * right_lo32,
		left_lo32 * right_hi32,
		left_hi32 * right_lo32,
		left_hi32 * right_hi32,
	};

	u64 carry = (((product[0] >> 32) + // high bits of low x low
		      (product[1] & UINT32_C(~0)) + // low bits of low x high
		      (product[2] & UINT32_C(~0))) // low bits of high x low
		     >> 32); // we want the high bits of that sum

	// Assemble the final product from these components, adding the carry
	// bits to the high 64 bits.
	return (u128){
		.low = product[0] + (product[1] << 32) + (product[2] << 32),
		.high = product[3] + (product[1] >> 32) + (product[2] >> 32) +
			carry,
	};
}

// Multiply two signed 64 bit integers and get the 128 bit result.
i128 __c0_i64_mul(i64 lhs, i64 rhs)
{
	union {
		u64 u;
		i64 i;
	} cvt;

	cvt.i = lhs;
	u64 a = cvt.u;

	cvt.i = rhs;
	u64 b = cvt.u;

	i128 result = __c0_u64_mul(a, b);

	if (lhs < 0) {
		result.high -= rhs;
	}

	if (rhs < 0) {
		result.high -= lhs;
	}

	return result;
}

// TODO: implement wrapping 128 bit integer multiplication.

// C0 Instructions

// rd{i, u} {8, 16, 32, 64, 128}(&src)
inline static i8 rdi8(i8 *src)
{
	return *src;
}
inline static i16 rdi16(i16 *src)
{
	return *src;
}
inline static i32 rdi32(i32 *src)
{
	return *src;
}
inline static i64 rdi64(i64 *src)
{
	return *src;
}
inline static i128 rdi128(i128 *src)
{
	return *src;
}
inline static u8 rdu8(u8 *src)
{
	return *src;
}
inline static u16 rdu16(u16 *src)
{
	return *src;
}
inline static u32 rdu32(u32 *src)
{
	return *src;
}
inline static u64 rdu64(u64 *src)
{
	return *src;
}
inline static u128 rdu128(u128 *src)
{
	return *src;
}

// wr{i,u}{8,16,32,64,128}(&dst, src)
inline static void wri8(i8 *dst, i8 src)
{
	*dst = src;
}
inline static void wri16(i16 *dst, i16 src)
{
	*dst = src;
}
inline static void wri32(i32 *dst, i32 src)
{
	*dst = src;
}
inline static void wri64(i64 *dst, i64 src)
{
	*dst = src;
}
inline static void wri128(i128 *dst, i128 src)
{
	*dst = src;
}
inline static void wru8(u8 *dst, u8 src)
{
	*dst = src;
}
inline static void wru16(u16 *dst, u16 src)
{
	*dst = src;
}
inline static void wru32(u32 *dst, u32 src)
{
	*dst = src;
}
inline static void wru64(u64 *dst, u64 src)
{
	*dst = src;
}
inline static void wru128(u128 *dst, u128 src)
{
	*dst = src;
}

// add{i,u}{8,16,32,64,128}(lhs, rhs)
inline static i8 addi8(i8 lhs, i8 rhs)
{
	union {
		i8 i;
		u8 u;
	} cvt;

	cvt.i = lhs;
	u8 a = cvt.u;
	cvt.i = rhs;
	u8 b = cvt.u;
	u8 result = a + b;
	cvt.u = result;
	return cvt.i;
}
inline static i16 addi16(i16 lhs, i16 rhs)
{
	union {
		i16 i;
		u16 u;
	} cvt;

	cvt.i = lhs;
	u16 a = cvt.u;
	cvt.i = rhs;
	u16 b = cvt.u;
	u16 result = a + b;
	cvt.u = result;
	return cvt.i;
}
inline static i32 addi32(i32 lhs, i32 rhs)
{
	union {
		i32 i;
		u32 u;
	} cvt;

	cvt.i = lhs;
	u32 a = cvt.u;
	cvt.i = rhs;
	u32 b = cvt.u;
	u32 result = a + b;
	cvt.u = result;
	return cvt.i;
}
inline static i64 addi64(i64 lhs, i64 rhs)
{
	union {
		i64 i;
		u64 u;
	} cvt;

	cvt.i = lhs;
	u64 a = cvt.u;
	cvt.i = rhs;
	u64 b = cvt.u;
	u64 result = a + b;
	cvt.u = result;
	return cvt.i;
}
inline static u8 addu8(u8 lhs, u8 rhs)
{
	return lhs + rhs;
}
inline static u16 addu16(u16 lhs, u16 rhs)
{
	return lhs + rhs;
}
inline static u32 addu32(u32 lhs, u32 rhs)
{
	return lhs + rhs;
}
inline static u64 addu64(u64 lhs, u64 rhs)
{
	return lhs + rhs;
}
inline static u128 addu128(u128 lhs, u128 rhs)
{
	return __c0_i128_add(lhs, rhs);
}

// sub{i,u}{8,16,32,64,128}(lhs, rhs)
inline static i8 subi8(i8 lhs, i8 rhs)
{
	union {
		i8 i;
		u8 u;
	} cvt;

	cvt.i = lhs;
	u8 a = cvt.u;
	cvt.i = rhs;
	u8 b = cvt.u;
	u8 result = a - b;
	cvt.u = result;
	return cvt.i;
}
inline static i16 subi16(i16 lhs, i16 rhs)
{
	union {
		i16 i;
		u16 u;
	} cvt;

	cvt.i = lhs;
	u16 a = cvt.u;
	cvt.i = rhs;
	u16 b = cvt.u;
	u16 result = a - b;
	cvt.u = result;
	return cvt.i;
}
inline static i32 subi32(i32 lhs, i32 rhs)
{
	union {
		i32 i;
		u32 u;
	} cvt;

	cvt.i = lhs;
	u32 a = cvt.u;
	cvt.i = rhs;
	u32 b = cvt.u;
	u32 result = a - b;
	cvt.u = result;
	return cvt.i;
}
inline static i64 subi64(i64 lhs, i64 rhs)
{
	union {
		i64 i;
		u64 u;
	} cvt;

	cvt.i = lhs;
	u64 a = cvt.u;
	cvt.i = rhs;
	u64 b = cvt.u;
	u64 result = a - b;
	cvt.u = result;
	return cvt.i;
}
inline static i128 subi128(i128 lhs, i128 rhs)
{
	return __c0_i218_sub(lhs, rhs);
}
inline static u8 subu8(u8 lhs, u8 rhs)
{
	return lhs - rhs;
}
inline static u16 subu16(u16 lhs, u16 rhs)
{
	return lhs - rhs;
}
inline static u32 subu32(u32 lhs, u32 rhs)
{
	return lhs - rhs;
}
inline static u64 subu64(u64 lhs, u64 rhs)
{
	return lhs - rhs;
}
inline static i128 subu128(i128 lhs, i128 rhs)
{
	return __c0_i218_sub(lhs, rhs);
}

// mul{i,u}{8,16,32,64,128}(lhs, rhs)
static inline i8 muli8(i8 lhs, i8 rhs)
{
	union {
		i8 i;
		u8 u;
	} cvt;

	cvt.i = lhs;
	u8 a = cvt.u;

	cvt.i = rhs;
	u8 b = cvt.u;

	cvt.u = a * b;
	return cvt.i;
}
static inline i16 muli16(i16 lhs, i16 rhs)
{
	union {
		i16 i;
		u16 u;
	} cvt;

	cvt.i = lhs;
	u16 a = cvt.u;

	cvt.i = rhs;
	u16 b = cvt.u;

	cvt.u = a * b;
	return cvt.i;
}
static inline i32 muli32(i32 lhs, i32 rhs)
{
	union {
		i32 i;
		u32 u;
	} cvt;

	cvt.i = lhs;
	u32 a = cvt.u;

	cvt.i = rhs;
	u32 b = cvt.u;

	cvt.u = a * b;
	return cvt.i;
}
static inline i64 muli64(i64 lhs, i64 rhs)
{
	union {
		i64 i;
		u64 u;
	} cvt;

	cvt.i = lhs;
	u64 a = cvt.u;

	cvt.i = rhs;
	u64 b = cvt.u;

	cvt.u = a * b;
	return cvt.i;
}
// TODO: muli128
static inline u8 mulu8(u8 lhs, u8 rhs)
{
	return lhs * rhs;
}
static inline u16 mulu16(u16 lhs, u16 rhs)
{
	return lhs * rhs;
}
static inline u32 mulu32(u32 lhs, u32 rhs)
{
	return lhs * rhs;
}
static inline u64 mulu64(u64 lhs, u64 rhs)
{
	return lhs * rhs;
}
// TODO: mulu128
