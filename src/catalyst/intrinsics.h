#ifndef catalyst_intrinsics_h
#define catalyst_intrinsics_h

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

#endif
