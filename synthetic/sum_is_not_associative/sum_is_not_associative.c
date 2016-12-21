// Performs the summation of array data in to ways:
//    ((((0 + data[0]) + data[1]) + data[2]) + ... )
//    (0 + (data[0] + (data[1] + (data[2] + ... ))))
// and checks that they are equivalent.  In general they
// need not be, since floating-point addition is not
// associative.
//
// This program should be deemed incorrect.
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
  FLOAT_TYPE sum = (FLOAT_TYPE)0;
  for(int i = 0; i < N; i++) {
    sum = sum + data[i];
  }
  return sum;
}

FLOAT_TYPE sum2(FLOAT_TYPE data[N]) {
  FLOAT_TYPE sum = (FLOAT_TYPE)0;
  for(int i = N-1; i >= 0; i--) {
    sum = data[i] + sum;
  }
  return sum;
}


int main() {

  FLOAT_TYPE data[N];
  
#ifdef KLEE
  klee_make_symbolic(data, sizeof(FLOAT_TYPE)*N, "data");
#else
  // This will lead to failure FLOAT_TYPE is float or double, and N is 5
  for(long long i = 0; i < N; i++) {
    data[i] = ((FLOAT_TYPE)(i*i)) + ((FLOAT_TYPE) 1.0)/(N + 1);
  }
#endif

  FLOAT_TYPE result1 = sum1(data);
  FLOAT_TYPE result2 = sum2(data);

  // DL: Note for isnan() we do bitwise and rather than short-circut to avoid
  // forking unnecessarily which adds unnecessary paths to explore.
  assert((isnan(result1) & isnan(result2)) || (result1 == result2));

  return 0;

}
