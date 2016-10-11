#ifdef KLEE
#include "klee/klee.h"
#endif

#include <gsl/gsl_rng.h>
#include <gsl/gsl_sort_double.h>

#include <assert.h>
#include <math.h>

#ifndef SMALL_LENGTH
#error Define SMALL_LENGTH, the number of smallest elements to be sorted
#endif

#ifndef LENGTH
#error Define LENGTH, the size of the array; must be >= SMALL_LENGTH
#endif

int
main (void)
{
#ifndef KLEE
  const gsl_rng_type * T;
  gsl_rng * r;
#endif

  size_t i, k = SMALL_LENGTH;
  size_t N = LENGTH;

  double * x = malloc (N * sizeof(double));
  double * small = malloc (k * sizeof(double));

#ifndef KLEE
  gsl_rng_env_setup();

  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  for (i = 0; i < N; i++)
    {
      x[i] = gsl_rng_uniform(r);
    }
#else
    klee_make_symbolic(x, N*sizeof(double), "x");
#endif

  gsl_sort_smallest (small, k, x, 1, N);

#ifndef KLEE
  printf ("%zu smallest values from %zu\n", k, N);
#endif

#ifndef KLEE
  for (i = 0; i < k; i++)
    {
#ifndef KLEE
      printf ("%zu: %.18f\n", i, small[i]);
#endif
    }
#endif

#if BUG
    // Mess up the sorting by swapping the first two elements of the result 
    double temp = small[0];
    small[0] = small[1];
    small[1] = temp;
#endif

  for(i = 1; i < k; i++)
    {
      assert(isnan(small[i-1]) || isnan(small[i]) || (small[i-1] <= small[i]));
    }

  free (x);
  free (small);
#ifndef KLEE
  gsl_rng_free (r);
#endif
  return 0;
}

