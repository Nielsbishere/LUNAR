#pragma once

#include <cstdint>

typedef int8_t i8;
typedef uint8_t u8;

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define u8_MAX (u8)0xFF
#define u8_MIN (u8)0
#define i8_MAX (i8)0x7F
#define i8_MIN (i8)0x80

#define u16_MAX (u16)0xFFFF
#define u16_MIN (u16)0
#define i16_MAX (i16)0x7FFF
#define i16_MIN (i16)0x8000

#define u32_MAX (u32)0xFFFFFFFF
#define u32_MIN (u32)0
#define i32_MAX (i32)0x7FFFFFFF
#define i32_MIN (i32)0x80000000

#define u64_MAX (u64)0xFFFFFFFFFFFFFFFF
#define u64_MIN (u64)0
#define i64_MAX (i64)0x7FFFFFFFFFFFFFFF
#define i64_MIN (i64)0x8000000000000000

struct maxVal{
	static f32 __f32_MAX() {
		u32 max = 0x7f7fffff;
		return *(f32*)&max;
	}
	static f64 __f64_MAX() {
		u64 max = 0x7FEFFFFFFFFFFFFF;
		return *(f64*)&max;
	}
};
#define f32_MAX maxVal::__f32_MAX()
#define f32_MIN -f32_MAX
#define f64_MAX maxVal::__f64_MAX()
#define f64_MIN -f64_MAX

#include "../../Headers/Defines.h"