// Tries to find numbers x and y such that adding y to x N times gives
// successively different results, but adding y an (N+1)th time leaves
// the result unchanged.  We say that the first N additions are not
// absorbed, but the final addition is absorbed.
//
// The benchmark should be deemed incorrect, as it asserts the
// non-existence of such x and y, yet they always exist (for
// non-pathological N).
//
// Defines:
// FLOAT_TYPE - float or double

// N          - The number of non-absorbed additions to be performed
//              before the absorbed addition.

#include <math.h>
#include <assert.h>
#ifdef KLEE
#include "klee/klee.h"
#endif

#ifndef N
#error "Define dimension of array, N"
#endif

#ifndef FLOAT_TYPE
#error "Define FLOAT_TYPE to be float or double"
#endif

#define FLOAT_LITERAL(X) (FLOAT_TYPE)X

int main() {

  FLOAT_TYPE x;
  FLOAT_TYPE y;

#ifdef KLEE
  klee_make_symbolic(&x, sizeof(FLOAT_TYPE), "x");
  klee_make_symbolic(&y, sizeof(FLOAT_TYPE), "y");
#else
  // These values cause the assert to fail for N=3
  x = 268435408.0f;
  y = 16.0f;
#endif

  // We are not interested in NaNs for this benchmark
  if(isnan(x) || isnan(y)) {
    return 0;
  }

  // Record the previous value in the summation
  FLOAT_TYPE last = x;
  for(int i = 0; i < N; i++) {
    // Compute the next value in the summation
    FLOAT_TYPE next = last + y;
    // If we have saturated, bail out; this is not interesting for the
    // benchmark
    if(next == last) {
      return 0;
    }
    // Get readyq to move on to nexts summation
    last = next;
  }
  // We want to see whether a final addition of y will be absorbed, so
  // we assert that this will not happen
  assert(last + y != last);
  return 0;

}
