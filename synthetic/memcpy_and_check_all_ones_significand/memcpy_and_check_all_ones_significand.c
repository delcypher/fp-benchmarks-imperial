#ifdef KLEE
#include "klee/klee.h"
#endif
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>


#ifndef FLOAT_TYPE
#error FLOAT_TYPE must be set
#else

#define EXP_MASK_float 0x7f800000
#define SIGNIFICAND_MASK_float 0x007fffff
#define INTEGER_TYPE_float uint32_t

#define EXP_MASK_double 0x7ff0000000000000LL
#define SIGNIFICAND_MASK_double 0x000fffffffffffffLL
#define INTEGER_TYPE_double uint64_t

#endif

#define _CONCAT(X, Y) X ## Y
#define _EXPAND(X, Y) _CONCAT(X, Y)
#define EXPAND(X) _EXPAND(X ## _ , FLOAT_TYPE)

int main(int argc, char** argv) {
  FLOAT_TYPE f;
#ifdef KLEE
  klee_make_symbolic(&f, sizeof(FLOAT_TYPE), "f");
#else
  f = 0;
#endif
  assert(sizeof(EXPAND(INTEGER_TYPE)) == sizeof(FLOAT_TYPE));

  // Do some operation that means that f is more
  // than just a read of some bytes. It now is
  // an expression of type FLOAT_TYPE.
  f = f + 1;

  EXPAND(INTEGER_TYPE) x = 0;
  memcpy(&x, &f, sizeof(FLOAT_TYPE));

  // copy back
  FLOAT_TYPE g;
  memcpy(&g, &x, sizeof(FLOAT_TYPE));

  if ( (x & EXPAND(EXP_MASK)) == EXPAND(EXP_MASK)) {
    // Number is either NaN or infinity
    if ( (x & EXPAND(SIGNIFICAND_MASK)) == 0) {
      // Number should be infinity
      assert(isinf(f) != 0);
      assert(isinf(g) != 0);
    } else {
      // Number should be NaN
      assert(isnan(f));
      assert(isnan(g));
    }
  } else {
    // Must be finite number
    assert(isfinite(f));
    assert(isfinite(g));
  }
  return 0;
}
