#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char  byte;
/*
typedef short          int16;
typedef (unsigned int16) uint16;
typedef long long      int64
typedef unsigned int64 uint64;
*/
typedef int            int32;
typedef unsigned       uint32;
#if defined(_MSC_VER)
typedef __int64        int64;
typedef unsigned __int64 uint64;
#else
typedef long           int64;
typedef unsigned long uint64;
#endif

#include <iostream>

#if defined(_MSC_VER)
#define FAIL() \
		__debugbreak(); \
	    exit(1)
#else
#define FAIL() \
	    exit(1)
#endif

#define assert(B) \
  if (!(B)) { \
    std::cerr << "ASSERTION FAILED(" << __FILE__ \
	      << ":" << __LINE__ << "): " << #B << std::endl;	\
	FAIL(); \
  }

#define assertm(B, m) \
  if (!(B)) {					\
    std::cerr << "ASSERTION FAILED(" << __FILE__ \
	      << ":" << __LINE__ << "): " << #B << std::endl;	\
    std::cerr << m << std::endl;			\
	FAIL(); \
  }




#define DISABLE_COPY(T) \
  inline T(const T&) { assert(!"Unreached: copy-ctor for: " #T); } \
  inline T& operator=(const T&) { assert(!"Unreached: copy-ctor for: " #T); }

#endif  // _TYPES_H_
