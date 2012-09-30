
#pragma once

#include <iostream>

typedef unsigned int  uint;
typedef unsigned char byte;

#if 1
#define SUBSQUARE_SIZE      3
#define SQUARE_SIZE         9
#define LOG2_SQUARE_SIZE    4
#define FULL_MASK           (((unsigned)0xFFFFFFFF) >> (32-SQUARE_SIZE))

#else
#define SUBSQUARE_SIZE      2
#define SQUARE_SIZE         4
#define LOG2_SQUARE_SIZE    3
#define FULL_MASK           (((unsigned)0xFFFFFFFF) >> (32-SQUARE_SIZE))

#endif


#if defined(WINDOWS)

#include <intrin.h>

#else

// QUESTION: How to trigger a debugger break in a *nix environment?
// QUESTION: Is JIT debugging possible on *nix?
#define __debugbreak()  exit(-5)

// QUESTION: GCC rdtsc intrinsic?
#define __rdtsc() 0

#endif


#if defined(RELEASE)

#define assert(x)

#else

#define assert(x) \
   if (!(x)) \
   { \
      std::cout << "ASSERT FAILED: " << #x << std::endl; \
      \
      __debugbreak(); \
   }
#endif

#define ispow2(x)  ((x) == 0) ? false : (((x)&((x)-1)) == 0)
