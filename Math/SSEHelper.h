#ifndef SSE_HELPER__H
#define SSE_HELPER__H

#include <intrin.h>

#define SWZ(v, a, b, c, d) _mm_shuffle_ps(v, v, _MM_SHUFFLE(a, b, c, d))

#endif