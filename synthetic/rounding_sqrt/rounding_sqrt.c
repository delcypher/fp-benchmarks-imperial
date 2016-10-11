#ifdef KLEE
#include "klee/klee.h"
#endif
#include <assert.h>
#include <fenv.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // For memcpy()

#ifndef BUG
#error BUG must be defined to be 0 or 1
#endif

typedef struct {
  double rne;
  double ru;
  double rd;
  double rz;
} Result;

void setRoundingMode(int rm) {
  int result = fesetround(rm);
  assert(result == 0);
  int newRoundingMode = fegetround();
  assert(newRoundingMode != -1);
  assert(newRoundingMode == rm);
}

int isNaNOrInf(double d) {
  return isnan(d) | isinf(d);
}

double toDouble(uint64_t data) {
  assert(sizeof(uint64_t) == sizeof(double));
  double initialValue = 0.0;
  memcpy(&initialValue, &data, sizeof(double));
  return initialValue;
}

uint64_t toBitsFromDouble(double d) {
  assert(sizeof(uint64_t) == sizeof(double));
  uint64_t toReturn;
  memcpy(&toReturn, &d, sizeof(double));
  return toReturn;
}

void printFloat(const char* lead, double d) {
  printf("%s: %.40e (hexfloat: %a) (bits: 0x%" PRIx64 ")\n", lead, d, d, toBitsFromDouble(d));
  fflush(stdout);
}

// This is a hack to prevent clang from optimizing
// the calls to sqrt() such that the rounding mode has
// no effect.
__attribute__((noinline)) double callSqrt(double d) {
  double temp = sqrt(d);
  return temp;
}

int main(int argc, char **argv) {
  // This value should trigger the bug and was found using
  // LibFuzzer.
  double initialValue = 0x1.727782de12fafp-365;

#ifdef KLEE
  klee_make_symbolic(&initialValue, sizeof(double), "initialValue");
#else
  printFloat("Initial Value", initialValue);
#endif

  if (isnan(initialValue) || isinf(initialValue)) {
    // Not interesting here
    return 0;
  }

  Result r;
  setRoundingMode(FE_TONEAREST);
  r.rne = callSqrt(initialValue);
  setRoundingMode(FE_UPWARD);
  r.ru = callSqrt(initialValue);
  setRoundingMode(FE_DOWNWARD);
  r.rd = callSqrt(initialValue);
  setRoundingMode(FE_TOWARDZERO);
  r.rz = callSqrt(initialValue);
#ifndef KLEE
  printFloat("Result RNE", r.rne);
  printFloat("Result RU ", r.ru);
  printFloat("Result RD ", r.rd);
  printFloat("Result RZ ", r.rz);
#endif
  setRoundingMode(FE_TONEAREST);

  // sqrt() of -ve number should give a NaN
  if (initialValue < 0.0) {
    assert(isnan(r.rne));
    assert(isnan(r.ru));
    assert(isnan(r.rd));
    assert(isnan(r.rz));
    return 0;
  }

  assert(!isNaNOrInf(r.rne));
  assert(!isNaNOrInf(r.ru));
  assert(!isNaNOrInf(r.rd));
  assert(!isNaNOrInf(r.rz));


  // sqrt() should not return a negative value so this should
  // always hold.
  assert(r.rd == r.rz);

#if !BUG
  if ((r.ru * r.ru) != initialValue && !(r.ru == r.rd & r.ru == 0.0)) {
    // InitialValue is not a perfect square and result wasn't zero
    // (could happen because value being square rooted was very small
    // or it was zero).
#else
  if (1) {
#endif
    assert(r.ru > r.rd);
  }

#if !BUG
  assert(r.rne == r.ru | r.rne == r.rd);
#else
  assert(r.rne == r.ru);
#endif
  return 0;
}
