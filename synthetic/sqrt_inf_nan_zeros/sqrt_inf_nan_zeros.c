#ifdef KLEE
#include "klee/klee.h"
#endif
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

#if FLOAT_WIDTH == 32
#define FLOAT_TYPE float
#define FLOAT_ZERO 0.0f
#define FLOAT_SQRT_FUNC sqrtf
#elif FLOAT_WIDTH == 64
#define FLOAT_TYPE double
#define FLOAT_ZERO 0.0
#define FLOAT_SQRT_FUNC sqrt
#elif FLOAT_WIDTH == 80
#define FLOAT_TYPE long double
#define FLOAT_ZERO 0.0l
#define FLOAT_SQRT_FUNC sqrtl
#else
#error UNKNOWN FLOAT_WIDTH
#endif

int main() {
  FLOAT_TYPE x = FLOAT_ZERO;
#ifdef KLEE
  klee_make_symbolic(&x, sizeof(FLOAT_TYPE), "x");
#endif
  FLOAT_TYPE result = FLOAT_SQRT_FUNC(x);

  if (isnan(x)) {
    assert(isnan(result));
    return 0;
  }

  if (x < FLOAT_ZERO) {
    assert(isnan(result));
    return 0;
  }

  if (isinf(x)) {
    assert(isinf(result) == 1);
    return 0;
  }

  if (x == FLOAT_ZERO) {
    assert(result == FLOAT_ZERO);
    // check sign bit sqrt(-0.0) == -0.0
    if (signbit(x)) {
      assert(signbit(result));
    }
    return 0;
  }

  assert(x > FLOAT_ZERO);
  return 0;
}
