#pragma once

// Unsigned int types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// Signed int types
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

// Floating point types
typedef float f32;
typedef double f64;

// Boolean types
typedef int b32;
typedef unsigned char b8;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define STATIC_ASSERT _Static_assert
#else
    #error "C11 or newer is required for static assertions"
#endif

// Make sure all types are of size
STATIC_ASSERT(sizeof(u8) == 1, "u8 must be 8-bit for binary compatability.");
STATIC_ASSERT(sizeof(u16) == 2, "u16 must be 16-bit for binary compatability.");
STATIC_ASSERT(sizeof(u32) == 4, "u32 must be 32-bit for binary compatability.");
STATIC_ASSERT(sizeof(u64) == 8, "u64 must be 64-bit for binary compatability.");

STATIC_ASSERT(sizeof(i8) == 1, "i8 must be 8-bit for binary compatability.");
STATIC_ASSERT(sizeof(i16) == 2, "i16 must be 16-bit for binary compatability.");
STATIC_ASSERT(sizeof(i32) == 4, "i32 must be 32-bit for binary compatability.");
STATIC_ASSERT(sizeof(i64) == 8, "i64 must be 64-bit for binary compatability.");

STATIC_ASSERT(sizeof(f32) == 4, "f32 must be 32-bit for binary compatability.");
STATIC_ASSERT(sizeof(f64) == 8, "f64 must be 64-bit for binary compatability.");

STATIC_ASSERT(sizeof(b32) == 4, "b32 must be 32-bit for binary compatability.");
STATIC_ASSERT(sizeof(b8) == 1, "b8 must be 8-bit for binary compatability.");

#define TRUE 1
#define FALSE 0

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define RQ_PLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "64-bit is required on Windows... How do you not have it Markas?"
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
    // Linux OS
    #define RQ_PLATFORM_LINUX 1
    #if defined(__ANDROID__)
        #define RQ_PLATFORM_ANDROID 1
    #endif
#elif defined(__APPLE__)
    // Apple platforms
    #define RQ_PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        // iOS simulator
        #define RQ_PLATFORM_IOS 1
        #define RQ_PLATFORM_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #define RQ_PLATFORM_IOS 1
    // iOS device 
    #elif TARGET_OS_MAC
    // Other kinds of Mac OS
    #else 
        #error "unknown Apple platform"
    #endif
#else
    #error "Markas... Oh Markas... Why are you on a platform I can't fucking recognize?!"
#endif

#ifdef RQ_EXPORT
    #define RAPI __declspec(dllexport)
#else
    #define RAPI
#endif
