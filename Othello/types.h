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

#include <iostream>

#define assert(B) \
  if (!(B)) { \
    std::cerr << "ASSERTION FAILED(" << __FILE__ \
	      << ":" << __LINE__ << "): " << #B << std::endl;	\
    exit(1); \
  }

#define assertm(B, m) \
  if (!(B)) {					\
    std::cerr << "ASSERTION FAILED(" << __FILE__ \
	      << ":" << __LINE__ << "): " << #B << std::endl;	\
    std::cerr << m << std::endl;			\
    exit(1); \
  }




#define DISABLE_COPY(T) \
  inline T(const T&) { assert(!"Unreached: copy-ctor for: " #T); } \
  inline T& operator=(const T&) { assert(!"Unreached: copy-ctor for: " #T); }

#endif  // _TYPES_H_
