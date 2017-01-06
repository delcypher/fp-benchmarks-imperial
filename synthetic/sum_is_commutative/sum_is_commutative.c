// Performs the summation of array data in to ways:
//    ((((0 + data[0]) + data[1]) + data[2]) + ... )
//    ( ... + (data[2] + (data[1] + (data[0] + 0))))
// and checks that they are equivalent.  They should be,
// since floating-point addition is commutative.
//
// This program should be deemed correct.
//
// Defines:
// FLOAT_TYPE - float or double
// N - the length of the array to be summed

#include <stdbool.h>
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

FLOAT_TYPE sum1(FLOAT_TYPE data[N]) {
  FLOAT_TYPE sum = (FLOAT_TYPE)0.0f;
  for(int i = 0; i < N; i++) {
    sum = sum + data[i];
  }
  return sum;
}

FLOAT_TYPE sum2(FLOAT_TYPE data[N]) {
  FLOAT_TYPE sum = (FLOAT_TYPE)0.0f;

// This is kind of clumsy but it
// is here to work around a bug clang 3.4
// where the debug information on `data[i]`
// is wrong when it is inside the loop head
// which is split accross multiple lines.
#if BUG == 0
#define LOOP_BOUND(i) N
#else
#define LOOP_BOUND(i) data[i]
#endif
  for(int i = 0; i < LOOP_BOUND(i) ; ++i) {
    sum = data[i] + sum;
  }
  return sum;
#undef LOOP_BOUND
}


int main(int argc, char** argv) {

  FLOAT_TYPE data[N];
  
#ifdef KLEE
  klee_make_symbolic(data, sizeof(FLOAT_TYPE)*N, "data");
#else
  for(int i = 0; i < N; i++) {
    data[i] = (FLOAT_TYPE)i;
  }
#endif

  FLOAT_TYPE result1 = sum1(data);
  FLOAT_TYPE result2 = sum2(data);

  // DL: Note for isnan() we do bitwise and rather than short-circut to avoid
  // forking unnecessarily which adds unnecessary paths to explore.
  // DL: KLEE really struggles checking `==` assert. This looks like
  // a place to see if its possible to optimize the query.
  assert((isnan(result1) & isnan(result2)) || (result1 == result2));

  return 0;

}
