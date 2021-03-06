// Performs an additive computation using double-precision arithmetic
// and single-precision interval arithmetic, and asserts that the
// double-precision result is contained in the single-precision
// interval.
//
// Defines:
// BUG - if 0 then there is no bug and the program should be deemed correct
//     - if 1 then there is a bug (the rounding during interval
//       addition is wrong) and the program should be deemed incorrect
// N   - Specifies the number of additions to be performed.

// This program sets the rounding mode and so must set this pragma to tell the
// compiler we will be using/modifying the floating point environment.
// Otherwise the program would be undefined (See 7.6.1 The FENV_ACCESS pragama
// - C11 specification).
#pragma STDC FENV_ACCESS ON
#include <assert.h>
#include <fenv.h>
#include <math.h>
#include <stdbool.h>
#ifdef KLEE
#include "klee/klee.h"
#endif

#ifndef N
#error "Define N, the number of additions to be performed."
#endif

#ifndef BUG
#error "Define BUG to be 0 or 1"
#endif

// A single-precision interval
typedef struct interval_s {
  float lower;
  float upper;
} interval_t;

// Neither component of an interval should be NaN, and lower and upper should be ordered.
bool well_formed_interval(interval_t a) {
  return !isnan(a.lower) && !isnan(a.upper) && a.lower <= a.upper;
}

interval_t add_intervals(interval_t a, interval_t b) {
  assert(well_formed_interval(a));
  assert(well_formed_interval(b));
  int old_mode = fegetround(); // Save the rounding mode
  fesetround(FE_DOWNWARD); // Set to round down, to compute lower bound for interval
  float lower = a.lower + b.lower;
#if !BUG
  fesetround(FE_UPWARD); // Set to round up, to record upper bound for interval
#endif
  float upper = a.upper + b.upper;
  fesetround(old_mode); // Restore the original rounding mode
  interval_t result = { lower, upper }; // The result is the interval comprised of the computed lower bound and the computed upper bound
  return result;
}

// Determines whether the given double-precision number is inside the single-precision interval.
bool in_interval(double x, interval_t a) {
  assert(!isnan(x));
  assert(well_formed_interval(a));
  return x >= (double)a.lower && x <= (double)a.upper;
}

int main(int argc, char** argv) {

  // The program adds 'increment' to 'initial' N times.
  // With e.g. N=7 and BUG=1, these values cause failure
  float initial = 10.0f;
  float increment = 5000000.0f;

  // The addition is performed relatively precisely, using a double,
  // as well as imprecisely, using a single-precision interval
  double precise;
  interval_t imprecise;
  
#ifdef KLEE
  klee_make_symbolic(&initial, sizeof(float), "initial");
#if SYMBOLIC_INCREMENT
  klee_make_symbolic(&increment, sizeof(float), "increment");
#endif
#endif

  // We are not interested in NaNs for this benchmark
  if(isnan(initial) || isnan(increment)) {
    return 0;
  }

  // Avoid infinities otherwise we'll eventually we
  // might do  "inf + inf" which will give NaN.
  if (isinf(initial) || isinf(increment)) {
    return 0;
  }

  // Initialise the double and the interval
  precise = (double)initial;
  imprecise.lower = initial;
  imprecise.upper = initial;
  assert(well_formed_interval(imprecise));

  // Do the arithmetic and tests
  for(int i = 0; i < N; i++) {
    assert(in_interval(precise, imprecise));
    precise += (double)increment;
    interval_t increment_interval = { increment, increment };
    imprecise = add_intervals(imprecise, increment_interval);
  }
  assert(in_interval(precise, imprecise));
  return 0;

}
