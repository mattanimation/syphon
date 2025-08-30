#ifndef DEFINES_H
#define DEFINES_H

// unisnged in types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;


// signed int types
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

// floating point types
typedef float f32;
typedef double f64;

// boolean types
typedef int b32;
typedef char b8;


// properly define static assertions
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// ensure all types are of correct size
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

STATIC_ASSERT(sizeof(i8) == 1, "Expected u8 to be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "Expected u8 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected u8 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected u8 to be 8 bytes.");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

#define TRUE 1
#define FALSE 0


// platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on windows!"
#endif
#elif defined(__linix__) || defined(__gnu_linux__)
// LINUX OS
#define PLATFORM_LINUX 1
#define _POSIX_C_SOURCE 200809L
#elif defined(__unix__)
#define PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
#define PLATFORM_POSIX 1
#define _POSIX_C_SOURCE 200809L
#elif __APPLE__
#define PLATFORM_APPLE 1
#define _DARWIN_C_SOURCE   /* for if_data */
#include <TargetConditionals.h>
#endif


#ifdef SEXPORT
// exports
#ifdef _MSC_VER
#define SAPI __declspec(dllexport)
#else
#define SAPI __attribute__((visibility("default")))
#endif
#else
// imports
#ifdef _MSC_VER
#define SAPI __declspec(dllimport)
#else
#define SAPI
#endif
#endif

#endif