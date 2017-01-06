#include <assert.h>
#include <gsl/gsl_blas.h>
#include <math.h>
#include <stdio.h>

#ifdef KLEE
#include "klee/klee.h"
#endif


int main(int argc, char** argv)
{
  double a[] = { 0.11, 0.12, 0.13,
                 0.21, 0.22, 0.23 };

  double b[] = { 1011, 1012,
                 1021, 1022,
                 1031, 1032 };

  double c[] = { 0.00, 0.00,
                 0.00, 0.00 };

  gsl_matrix_view A = gsl_matrix_view_array(a, 2, 3);
  gsl_matrix_view B = gsl_matrix_view_array(b, 3, 2);
  gsl_matrix_view C = gsl_matrix_view_array(c, 2, 2);

#ifdef KLEE
  klee_make_symbolic(a, sizeof(a), "a");
  klee_assume(!isnan(a[0]));
  a[1] = 0.12;
  a[2] = 0.13;
  a[3] = 0.21;
  a[4] = 0.22;
  a[5] = 0.23;
#endif


  /* Compute C = A B */

  gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
                  1.0, &A.matrix, &B.matrix,
                  0.0, &C.matrix);
 
  assert(c[2] > 674.06 && c[2] < 674.07);
  assert(c[3] > 674.719 && c[3] < 674.73);

#ifndef BUG
  assert(c[0] == a[0]*b[0] + a[1]*b[2] + a[2]*b[4]);
  assert(c[1] == a[0]*b[1] + a[1]*b[3] + a[2]*b[5]);
#else
  assert(c[0] != 367.76);
  assert(c[1] != 368.12);
#endif

#ifndef KLEE
  printf ("[ %g, %g\n", c[0], c[1]);
  printf ("  %g, %g ]\n", c[2], c[3]);
#endif

  return 0;  
}
