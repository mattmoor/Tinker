
#pragma once

#include <iostream>

typedef unsigned int  uint;
typedef unsigned char byte;

#if 1
#define SUBSQUARE_SIZE      3
#define SQUARE_SIZE         9
#define LOG2_SQUARE_SIZE    4
#define FULL_MASK           (0xFFFFFFFFui32 >> (31-SQUARE_SIZE))

#else
#define SUBSQUARE_SIZE      2
#define SQUARE_SIZE         4
#define LOG2_SQUARE_SIZE    3
#define FULL_MASK           (0xFFFFFFFFui32 >> (31-SQUARE_SIZE))

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