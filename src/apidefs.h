#pragma once

// Reference: http://www.guyrutenberg.com/2007/11/19/c-goes-to-operator/
// useful for while loops

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  // nonstandard extension used : nameless struct/union
#endif


#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE inline
#endif

#include <cstdint>