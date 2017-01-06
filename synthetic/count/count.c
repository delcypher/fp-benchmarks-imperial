// This example counts the number of single-precision floating-point
// numbers between two bounding single-precision floating-point
// numbers.  The bounding numbers are required to be sufficiently
// close that we know an upper bound on the count.  The benchmark
// checks that the count does not exceed this bound.

#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define START_FLOAT 1.0f
#define END_FLOAT 1.00001f
#if BUG
#define FLOATS_BETWEEN 20
#else
#define FLOATS_BETWEEN 84
#endif

#ifdef KLEE
#include "klee/klee.h"
#endif

int main(int argc, char** argv) {

  // The program counts all floats between start (inclusive) and end
  // (exclusive)
  float start;
  float end;

#ifdef KLEE
  klee_make_symbolic(&start, sizeof(float), "start");
  klee_make_symbolic(&end, sizeof(float), "end");
#else
  start = START_FLOAT;
  end = END_FLOAT;
#endif

  // We constrain the bounds to be numbers, ordered, and within the
  // range [START_FLOAT, END_FLOAT].  As a result, FLOATS_BETWEEN
  // gives a bound on the number of single-precision floating-point
  // numbers that lie in between these bounds.

  if(isnan(start) || isnan(end)) {
    return 0;
  }

  if(start > end) {
    return 0;
  }
 
  if(start < START_FLOAT) {
    return 0;
  }

  if(end > END_FLOAT) {
    return 0;
  }

  int count = 0;
  
  // We require that unsigned and float have the same size for the
  // program to operate correctly
  assert(sizeof(unsigned) == sizeof(float));

  // Iterate through the floating-point numbers in the range by
  // bit-conversion to unsigned.
  for(float current = start; current != end; count++) {
    unsigned temp;
    memcpy(&temp, &current, sizeof(float));
    temp++;
    memcpy(&current, &temp, sizeof(float));
  }

  // Check that the count is non-negative and bounded above by
  // FLOATS_BETWEEN
#ifndef KLEE
  printf("Count is %d\n", count);
#endif
  assert(count >= 0);
  assert(count <= FLOATS_BETWEEN);
  return 0;
}
