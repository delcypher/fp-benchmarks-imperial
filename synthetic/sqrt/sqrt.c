// Computes the square root of a number using a naive binary search.
//
// The benchmark should be deemed correct: it is restricted to numbers
// within a certain range for which the square root function works
// well.
//
// No defines are required for this benchmark.

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#ifdef KLEE
#include "klee/klee.h"
#endif

#define EPSILON 0.0000001f

// Check whether two floats are within EPSILON of each other
bool almost_equal(float x, float y) {
  // DL: Need to implement fabs() internally.
  return fabs(x - y) < EPSILON;
}

float naive_sqrt(float x) {

  assert(x >= 0.0f);
  assert(!isnan(x));

  // Binary search for a number a with the property that a*a is almost
  // equal to x

  float lo = 0.0f;
  float hi = x;
  for(;;) {
    assert(lo <= hi);
    float candidate = (hi + lo)/2.0f;
    if(almost_equal(candidate
#if BUG==0
       *candidate
#endif
           , x)) {
      return candidate;
    }
    if(candidate*candidate > x) {
      hi = candidate;
    } else {
      lo = candidate;
    }
    assert(lo != hi);
  }

}


int main(int argc, char** argv) {

  float x;

#ifdef KLEE
  klee_make_symbolic(&x, sizeof(float), "x");
#else
  x = 5.0f;
#endif

  // Restrict to values that are numbers, and are within a small
  // range.

  if(isnan(x)) {
    return 0;
  }

  if(x < 0.0f || x > 100.0f) {
    return 0;
  }

  // Check that square root of x does not exceed x, and that the value
  // computed squares to give almost x.
  float sqrt_x = naive_sqrt(x);
  assert(sqrt_x <= x);
  assert(almost_equal(x, sqrt_x*sqrt_x));

  return 0;

}
