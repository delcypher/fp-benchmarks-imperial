// This program creates a diagonal matrix where the digonal has values
// 1, 2, 1, 2, ..., and asserts that the matrix has no inverse.  This
// is wrong, as the diagonal matrix with diagonal 1, 0.5, 1, 0.5, ...,
// is an inverse.  Hence the benchmark should be deemed incorrect.
//
// Defines:
// N - the dimension of the matrices to be worked with
// FLOAT_TYPE - float or double

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#ifdef KLEE
#include "klee/klee.h"
#endif

#ifndef N
#error "Define dimension of matrices, N"
#endif

#ifndef FLOAT_TYPE
#error "Define FLOAT_TYPE to be float or double"
#endif

#define FLOAT_LITERAL(X) (FLOAT_TYPE)X

#define EPSILON FLT_EPSILON

// Checks whether two numbers are close to one another
bool very_close(FLOAT_TYPE x, FLOAT_TYPE y) {
  // DL: We should implement fabs() internally in KLEE.
  // FIXME: This causes a crash in KLEE currently.
  return fabs(x - y) < EPSILON;
}

// Multiplies matrices A and B, putting the result in C
void matrix_mul(FLOAT_TYPE A[N][N], FLOAT_TYPE B[N][N], FLOAT_TYPE C[N][N]) {
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      FLOAT_TYPE sum = FLOAT_LITERAL(0.0);
      for(int k = 0; k < N; k++) {
	sum += A[i][k] * B[k][j];
      }
      C[i][j] = sum;
    }
  }
}

// Returns true if and only if matrices A and B multiply to give
// something very close to the identity matrix
bool check_inverses(FLOAT_TYPE A[N][N], FLOAT_TYPE B[N][N]) {
  FLOAT_TYPE C[N][N];
  matrix_mul(A, B, C);
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      if(i == j) {
	if(!very_close(C[i][j], FLOAT_LITERAL(1.0))) {
	  return false;
	}
      } else {
	if(!very_close(C[i][j], FLOAT_LITERAL(0.0))) {
	  return false;
	}
      }
    }
  }
  return true;
}

int main(int argc, char** argv) {
  FLOAT_TYPE A[N][N];
  // In bug mode, make a diagonal matrix, with the diagonal alternating between 1.0
  // and 2.0.
  // Otherwise, make the (non-invertible) zero matrix.
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
#if BUG==1 || !KLEE
      if(i == j) {
	if((i % 2) == 0) {
	  A[i][j] = FLOAT_LITERAL(1.0);
	} else {
	  A[i][j] = FLOAT_LITERAL(2.0);
	}
      } else {
	A[i][j] = FLOAT_LITERAL(0.0);
      }
#else
      A[i][j] = FLOAT_LITERAL(0.0);
#endif
    }
  }

  FLOAT_TYPE B[N][N];

#ifdef KLEE
  // Make the array B symbolic, so that we are in effect searching for
  // an inverse for A
  klee_make_symbolic(B, sizeof(FLOAT_TYPE)*N*N, "B");
#endif

  // For the bug case, here is the actual inverse: the diagonal alternates between 1 and
  // 0.5; if KLEE is enabled then we leave the diagonal symbolic.
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      if(i == j) {
#ifndef KLEE
	if((i % 2) == 0) {
	  B[i][j] = FLOAT_LITERAL(1.0);
	} else {
	  B[i][j] = FLOAT_LITERAL(0.5);
	}
#endif
      } else {
	B[i][j] = FLOAT_LITERAL(0.0);
      }
    }
  }

  assert(!check_inverses(A, B));
  return 0;

}
