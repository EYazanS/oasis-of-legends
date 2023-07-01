#ifndef catalyst_intrinsics_h
#define catalyst_intrinsics_h

#include <math.h>
#include "typesdef.h"

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
// TODO Moar compilerz!!!
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#if defined(COMPILER_MSVC)
//  Microsoft
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define DLL_IMPORT__declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define DLL_IMPORT
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

inline i32 round_real32_to_int32(r32 value)
{
	i32 result = (i32)roundf(value);
	return result;
}

inline BitScanResult find_least_sigifigant_set_bit(u32 value)
{
	BitScanResult result = {};
#if COMPILER_MSVC
	result.Found = _BitScanForward((unsigned long*)&result.Index, value);
#else
	for (u32 currentScanningIndex = 0; currentScanningIndex < 32; currentScanningIndex++)
	{
		if (value & 1 << currentScanningIndex)
		{
			result.Found = true;
			result.Index = currentScanningIndex;
			break;
		}
	}
#endif
	return result;
}

inline u32 rotate_left(u32 value, i32 amount)
{
#if COMPILER_MSVC
	u32 result = _rotl(value, amount);
#else
	amount &= 31;
	u32 result = (value << amount) | (value >> (32 - amount));
#endif
	return result;
}

#endif
