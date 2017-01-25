// Searches a sorted list using linear and binary search methods.  The
// benchmark can be made buggy by using the BUG macro, in which case
// the linear search is incorrect.
//
// Defines:
// BUG - if 0 then there is no bug and the program should be deemed correct
//     - if 1 then there is an error in the linear search and the program
//       should be deemed incorrect
// FLOAT_TYPE - float or double
// N   - the length of the array

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

bool is_sorted(FLOAT_TYPE data[N]) {
  // Returns true iff array is NaN-free and sorted
  for(int i = 0; i < N; i++) {
    if(isnan(data[i])) {
      return false;
    }
    if(i > 0 && data[i] < data[i-1]) {
      return false;
    }
  }
  return true;
}

bool linear_search(FLOAT_TYPE data[N], FLOAT_TYPE elem) {
  for(int i = 
#if BUG
     1
#else
     0
#endif
        ; i < N; i++) {
    if(data[i] == elem) {
      return true;
    }
  }
  return false;
}

bool binary_search(FLOAT_TYPE data[N], FLOAT_TYPE elem) {
  // Range under consideration is closed.
  // i.e. [lo, hi]
  int lo = 0;
  int hi = N - 1;
  while(lo <= hi) {
    int mid = (hi+lo) / 2;
    if(data[mid] == elem) {
      return true;
    }
    if(data[mid] > elem) {
      hi = mid - 1;
    } else {
      lo = mid + 1;
    }
  }
  return false;
}

int main(int argc, char** argv) {

  FLOAT_TYPE data[N];
  FLOAT_TYPE elem;
  
#ifdef KLEE
  klee_make_symbolic(data, sizeof(FLOAT_TYPE)*N, "data");
  klee_make_symbolic(&elem, sizeof(FLOAT_TYPE), "elem");
#else
  for(long long i = 0; i < N; i++) {
    data[i] = (FLOAT_TYPE)(i*i);
  }
  elem = (FLOAT_TYPE)0;
#endif

  if(!is_sorted(data)) {
    return 0;
  }

  assert(linear_search(data, elem) == binary_search(data, elem));

  return 0;

}
