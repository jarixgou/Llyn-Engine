#ifndef SSE_HELPER__H
#define SSE_HELPER__H

#include <intrin.h>

#define SWZ(v, a, b, c, d) _mm_shuffle_ps(v, v, _MM_SHUFFLE(a, b, c, d))
#define SWZ_1(v, a) SWZ(v, a, a, a, a)
#define VEC_SHUFFLE(vA, vB, a, b, c, d) _mm_shuffle_ps(vA, vB, _MM_SHUFFLE(a, b, c, d))

#endif