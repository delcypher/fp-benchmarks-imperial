#ifdef KLEE
#include "klee/klee.h"
#endif
#include <stdio.h>
#include <gsl/gsl_poly.h>

#include <assert.h>
#include <math.h>

int main (void)
{
  int i;
  
#ifdef HIGH_DEGREE
#define N 6
  /* coefficients of P(x) =  -1 + x^5  */
  double a[6] = { -1, 0, 0, 0, 0, 1 };
#if BUG
  // This values are just slightly larger than the known values for the roots
  double expected[2*(N-1)] = { -0.809016994374947672, +0.587785252292473360,
			       -0.809016994374947672, -0.587785252292473358,
			       +0.309016994374947508, +0.951056516295152977, 
			       +0.309016994374947508, -0.951056516295152975,
			       +0.999999999999999890, +0.000000000000000001 };
#endif

#else

#define N 2
  /* coefficients of P(x) =  1 - x  */
  double a[N] = { 1, -1 };
#if BUG
  double expected[2*(N-1)] = { 1.000000000000000001, 0.000000000000000001};
#endif
#endif

  double z[2*(N-1)];
  
#ifdef KLEE
  klee_make_symbolic(&a, sizeof(a), "a");

#ifndef HIGH_DEGREE
  a[1] = -1;
#else
  klee_assume(a[0] > -1.000000000000001);
  klee_assume(a[0] < -0.999999999999999);
  a[1] = 0;
  a[2] = 0;
  a[3] = 0;
  a[4] = 0;
  a[5] = 1;
#endif

#endif

  gsl_poly_complex_workspace * w 
      = gsl_poly_complex_workspace_alloc (N);
  
  gsl_poly_complex_solve (a, N, w, z);

  gsl_poly_complex_workspace_free (w);

#ifndef HIGH_DEGREE
  assert(isnan(z[0]) || z[0] == a[0]);
  //assert(z[0] > a[0] - 0.00001 && z[0] < a[0] + 0.00001);
  assert(z[1] == 0);
#endif

  for (i = 0; i < N-1; i++)
    {
#if BUG
      assert(z[2*i] > expected[2*i]);
      assert(z[2*i+1] > expected[2*i+1]);
#endif

#ifndef KLEE
      printf ("z%d = %+.18f %+.18f\n", i, z[2*i], z[2*i+1]);
#endif
    }

  return 0;
}
