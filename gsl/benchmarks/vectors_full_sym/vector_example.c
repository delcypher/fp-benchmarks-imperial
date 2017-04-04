#ifdef KLEE
#include "klee/klee.h"
#endif

#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <math.h>
#include <assert.h>

int
main(int argc, char** argv)
{
  int i;
  gsl_vector * v = gsl_vector_alloc (N);
  gsl_vector * a = gsl_vector_alloc (N);

  double base_v = 1.23;
  for (i = 0; i < N; i++) {
    gsl_vector_set(v, i, base_v + i);
    gsl_vector_set(a, i, base_v + 2*i);
  }

#ifdef KLEE
  // Make `v` vector completly symbolic
  for (i = 0; i < N; ++i) {
    double temp = 0.0;
    char temp_str[] = "v_X";
    temp_str[2] = '0' + i; // HACK: Only works if N <= 9
    klee_make_symbolic(&temp, sizeof(double), temp_str);
    gsl_vector_set(v, i, temp);
  }

  // Make `a` vector completly symbolic
  for (i = 0; i < N; ++i) {
    double temp = 0.0;
    char temp_str[] = "a_X";
    temp_str[2] = '0' + i; // HACK: Only works if N <= 9
    klee_make_symbolic(&temp, sizeof(double), temp_str);
    gsl_vector_set(a, i, temp);
  }
#endif

  gsl_vector * u = gsl_vector_alloc (N);
  gsl_vector * x = gsl_vector_alloc (N);
  gsl_vector * y = gsl_vector_alloc (N);

  gsl_vector_set_all(u, 0.0);
  // u = -v
  gsl_vector_sub(u, v);
  
  gsl_vector_memcpy(x, a);
  // x = a + v
  gsl_vector_add(x, v);

  gsl_vector_memcpy(y, a);
  // y = a - (-v)
  gsl_vector_sub(y, u);

  // Check `x` and `y` are the same.
  for (i = 0; i < N; ++i) {
    double x_i = gsl_vector_get(x, i);
    double y_i = gsl_vector_get(y, i);
    if (isnan(x_i)) {
      assert(isnan(y_i));
    } else {
      assert(x_i == y_i);
    }
  }

  gsl_vector_free(u);
  gsl_vector_free(v);
  gsl_vector_free(a);
  gsl_vector_free(x);
  gsl_vector_free(y);

  return 0;
}
