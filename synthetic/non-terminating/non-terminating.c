// This benchmark explores whether a program terminates or not when a
// loop counter is decremented by a runtime-fixed floating-point
// value.
//
// Defines:
// BUG - if 0 then there is no bug and the program should be
//       deemed correct; in this case the loop does not terminate
//     - if 1 then there is a bug and the program should be deemed
//       incorrect; this is because the loop terminates and the
//       assert(false) after the loop is hit

#include <math.h>
#include <assert.h>
#include <stdbool.h>
#ifdef KLEE
#include "klee/klee.h"
#endif

#ifndef BUG
#error "Define BUG to be 0 or 1"
#endif

#define HIGH 0.000003814698f
#define LOW 0.000003814697f
 
int main(int argc, char** argv) {
  
  // This is the amount by which the loop variable will be decremented
  // on each loop iteration.
  float delta;
  
#ifdef KLEE
  klee_make_symbolic(&delta, sizeof(float), "delta");
#else
#if BUG
  delta = HIGH;
#else
  delta = LOW;
#endif
  
#endif
  
  // We require delta to be a number, and constrain it according to
  // whether or not we want the loop to terminate.

  if(isnan(delta)) {
    return 0;
  }
  
#if BUG
  if(delta < HIGH) {
    return 0;
  }
#else
  if(delta > LOW) {
    return 0;
  }
#endif
 
  // Loop from 100.0f to 0.0f in steps of delta; if delta is too small
  // it will be absorbed and no decrement will occur
  for(float f = 100.0f; f > 0.0f; f -= delta);
  
  // There is a bug if the loop terminates
  assert(false);
  return 0;

}
