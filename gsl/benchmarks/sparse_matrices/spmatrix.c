#ifdef KLEE
#include "klee/klee.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <gsl/gsl_spmatrix.h>

int
main()
{
  gsl_spmatrix *A = gsl_spmatrix_alloc(5, 4); /* triplet format */
  gsl_spmatrix *C;
  size_t i, j;

  unsigned R0, R1, R2, R3, R4, R5, R6, R7;
  unsigned C0, C1, C2, C3, C4, C5, C6, C7;
  double V2;

  R0 = 0, C0 = 2;
  R1 = 0, C1 = 3;
  R2 = 1, C2 = 0;
  R3 = 1, C3 = 2;
  R4 = 3, C4 = 0;
  R5 = 3, C5 = 1;
  R6 = 3, C6 = 3;
  R7 = 4, C7 = 0;

  V2 = 1.0;

#ifdef KLEE
  klee_make_symbolic(&R0, sizeof(unsigned), "R0");
  klee_assume(R0 < 5);
  klee_make_symbolic(&R1, sizeof(unsigned), "R1");
  klee_assume(R1 < 5);
  klee_make_symbolic(&C7, sizeof(unsigned), "C7");
  klee_assume(C7 < 4);

  klee_make_symbolic(&V2, sizeof(double), "V2");
  klee_assume(V2 >= -1.0);
  klee_assume(V2 <= 1.0);
#endif

  /* build the sparse matrix */
  gsl_spmatrix_set(A, R0, C0, 3.1);
  gsl_spmatrix_set(A, R1, C1, 4.6);
  gsl_spmatrix_set(A, R2, C2, V2);
  gsl_spmatrix_set(A, R3, C3, 7.2);
  gsl_spmatrix_set(A, R4, C4, 2.1);
  gsl_spmatrix_set(A, R5, C5, 2.9);
  gsl_spmatrix_set(A, R6, C6, 8.5);
  gsl_spmatrix_set(A, R7, C7, 4.1);

#ifndef KLEE
  printf("printing all matrix elements:\n");
  for (i = 0; i < 5; ++i)
    for (j = 0; j < 4; ++j)
      printf("A(%zu,%zu) = %g\n", i, j,
             gsl_spmatrix_get(A, i, j));

  /* print out elements in triplet format */
  printf("matrix in triplet format (i,j,Aij):\n");
  for (i = 0; i < A->nz; ++i)
    printf("(%zu, %zu, %.1f)\n", A->i[i], A->p[i], A->data[i]);
#endif

  /* convert to compressed column format */
  C = gsl_spmatrix_compcol(A);

  // The following values are possible for C->i - they are what the default matrix values give
  int possible_C_i[] = { 1, 3, 4, 3, 0, 1, 0, 3 };

#ifndef KLEE
  printf("matrix in compressed column format:\n");
  printf("i = [ ");
#endif
  for (i = 0; i < C->nz; ++i) {
#if BUG
    assert(C->i[i] != possible_C_i[i]);
#endif
#ifndef KLEE
    printf("%zu, ", C->i[i]);
#endif
  }
#ifndef KLEE
  printf("]\n");

  printf("p = [ ");
  for (i = 0; i < C->size2 + 1; ++i)
    printf("%zu, ", C->p[i]);
  printf("]\n");

  printf("d = [ ");
  for (i = 0; i < C->nz; ++i)
    printf("%g, ", C->data[i]);
  printf("]\n");
#endif

  gsl_spmatrix_free(A);
  gsl_spmatrix_free(C);

  return 0;
}
