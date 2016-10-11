#ifdef KLEE
#include "klee/klee.h"
#endif

#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <math.h>
#include <assert.h>

int
main (void)
{
  int i;
  gsl_vector * v = gsl_vector_alloc (N);
  gsl_vector * a = gsl_vector_alloc (N);
 
  double base_v = 1.23;
  for (i = 0; i < N; i++)
    {
      gsl_vector_set (v, i, base_v + i);
    }

#ifdef KLEE
  double temp;
  klee_make_symbolic(&temp, sizeof(double), "temp");
#if BUG==0
  klee_assume(!isnan(temp));
#endif
  gsl_vector_set (v, 0, temp);
#endif 
 
   double base_a = 2.34;
   for (i = 0; i < N; i++)
    {
      gsl_vector_set (a, i, base_a + i);
    }
  
  gsl_vector * u = gsl_vector_alloc (N);
  gsl_vector * x = gsl_vector_alloc (N);
  gsl_vector * y = gsl_vector_alloc (N);

  gsl_vector_set_all(u, 0.0);
  gsl_vector_sub(u, v);
  
  gsl_vector_memcpy(x, a);
  gsl_vector_add(x, v);

  gsl_vector_memcpy(y, a);
  gsl_vector_sub(y, u);

  double min_x = gsl_vector_min(x);
  double max_x = gsl_vector_max(x);
  double min_y, max_y;
  gsl_vector_minmax(y, &min_y, &max_y);

  assert(min_x == min_y);
  assert(max_x == max_y);
  assert(min_x <= max_x);

  gsl_vector_free(u);
  gsl_vector_free(v);
  gsl_vector_free(a);
  gsl_vector_free(x);
  gsl_vector_free(y);

  return 0;
}
