// This program computes a prefix sum over an array of floating-point
// numbers, and asserts that (a) the prefix sum result is
// monotonically increasing, and (b) each element of the prefix sum
// result is larger than or equal to the corresponding element of the
// input.  This holds if every element of the input is non-negative.
//
// The program can be compiled in correct or buggy form; in correct
// form each element is assumed to be non-negative; in incorrect form
// this is not assumed for element 0.  If it is sufficiently negative
// it can spoil the postcondition of the program.
//
// Defines:
// BUG - if 0 then all elements of the input are non-negative and 
//       there is no bug
//     - if 1 then element 0 is not constrained, so there can be a 
//       bug
// N   - the dimension of the input and output arrays
// FLOAT_TYPE - float or double

#include <stdbool.h>
#include <math.h>
#include <assert.h>
#ifdef KLEE
#include "klee/klee.h"
#endif

#ifndef N
#error "Define dimension of arrays, N"
#endif

#ifndef FLOAT_TYPE
#error "Define FLOAT_TYPE to be float or double"
#endif

#ifndef BUG
#error "Must define BUG to be 0 or 1"
#endif

#define FLOAT_LITERAL(X) (FLOAT_TYPE)X

void prefix_sum(FLOAT_TYPE data_in[N], FLOAT_TYPE data_out[N]) {
  data_out[0] = data_in[0];
  for(int i = 1; i < N; i++) {
    data_out[i] = data_in[i] + data_out[i-1];
  }
}

int main() {

  FLOAT_TYPE data_in[N];
  FLOAT_TYPE data_out[N];
  
#ifdef KLEE
  klee_make_symbolic(data_in, sizeof(FLOAT_TYPE)*N, "data_in");
#else
  for(long long i = 0; i < N; i++) {
    data_in[i] = (FLOAT_TYPE)i;
  }
#if BUG
  data_in[0] = -10000000.0f;
#endif

#endif

  for(int i =
#if BUG
1
#else
0
#endif
	; i < N; i++) {
    if(isnan(data_in[i]) || data_in[i] < FLOAT_LITERAL(0.0)) {
      return 0;
    }
  }

  prefix_sum(data_in, data_out);

  for(int i = 0; i < N; i++) {
    assert(data_out[i] >= data_in[i]);
    if(i > 0) {
      assert(data_out[i] >= data_out[i-1]);
    }
  }

  return 0;

}
