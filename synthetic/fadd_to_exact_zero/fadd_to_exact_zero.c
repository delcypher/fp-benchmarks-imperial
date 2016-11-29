#include "klee/klee.h"
#include <assert.h>
#include <fenv.h>
#include <math.h>
#include <stdio.h>

#ifndef FLOAT_TYPE
#error FLOAT_TYPE must be defined
#endif

int main(int arc, char** argv) {
  FLOAT_TYPE f, g;
  klee_make_symbolic(&f, sizeof(f), "f");
  klee_make_symbolic(&g, sizeof(g), "g");

  // IEEE 754-2008: 6.3 The sign bit
  // When the sum of two operands with opposite signs (or the difference of two
  // operands with like signs) is exactly zero, the sign of that sum (or
  // difference) shall be +0 in all rounding-direction attributes except
  // roundTowardNegative; under that attribute, the sign of an exact zero sum
  // (or difference) shall be −0.  However, x + x = x − (−x) retains the same
  // sign as x even when x is zero.
  klee_assume(!isnan(f));
  klee_assume(!isnan(g));
  klee_assume(fabs(f) == fabs(g)); // Have same absolute value
  // Have opposite sign bits.
  klee_assume((signbit(f) & !signbit(g)) | (!signbit(f) & signbit(g)));

  // Fork on the rounding mode.
  unsigned int choice = 0;
  klee_make_symbolic(&choice, sizeof(int), "choice");
  klee_assume(choice < 4);
  int failed_set_rm = 0;
  switch (choice) {
    case 0:
      failed_set_rm = fesetround(FE_TONEAREST);
      printf("Setting rounding mode to FE_TONEAREST\n");
      break;
    case 1:
      failed_set_rm = fesetround(FE_UPWARD);
      printf("Setting rounding mode to FE_UPWARD\n");
      break;
    case 2:
      failed_set_rm = fesetround(FE_DOWNWARD);
      printf("Setting rounding mode to FE_DOWNWARD\n");
      break;
    case 3:
      failed_set_rm= fesetround(FE_TOWARDZERO);
      printf("Setting rounding mode to FE_TOWARDZERO\n");
      break;
    default:
      assert(0 && "unreachable");
  }
  assert(failed_set_rm == 0 && "failed to set rounding mode");

  FLOAT_TYPE result = f + g;

  if (isinf(f)) {
    // Adding infinites should give NaN.
    assert(isnan(result));
    return 0;
  }

  assert(result == 0.0f);

  if (fegetround() == FE_DOWNWARD) {
    // Should be -0.0
    printf("Checking FE_DOWNWARD\n");
    assert(result == 0.0f);
    assert(signbit(result));
  } else {
    // Should be +0.0
    assert(result == 0.0f);
    //printf("result: %f\n", result);
    assert(!signbit(result));
  }
  return 0;
}
