#ifdef KLEE
#include "klee/klee.h"
#endif

#include <stdio.h>
#include <gsl/gsl_statistics.h>

#include <assert.h>

int
main(int argc, char** argv)
{
  double data[5] = {17.2, 18.1, 16.5, 18.3, 12.6};
  double mean, variance, largest, smallest;

#ifdef KLEE
  klee_make_symbolic(&data[0], 5*sizeof(double), "data");
  for (int i = 0; i < 5; ++i) {
    klee_assume(data[i] >= 10.0);
    klee_assume(data[i] <= 20.0);
  }
#endif

  mean     = gsl_stats_mean(data, 1, 5);
  variance = gsl_stats_variance(data, 1, 5);
  largest  = gsl_stats_max(data, 1, 5);
  smallest = gsl_stats_min(data, 1, 5);

#ifndef KLEE
  printf ("The dataset is %g, %g, %g, %g, %g\n",
         data[0], data[1], data[2], data[3], data[4]);

  printf ("The sample mean is %g\n", mean);
  printf ("The estimated variance is %g\n", variance);
  printf ("The largest value is %g\n", largest);
  printf ("The smallest value is %g\n", smallest);
#endif

  assert(largest >= smallest);
  assert(largest >= 10.0);
  assert(largest <= 20.0);
  assert(smallest >= 10.0);
  assert(smallest <= 20.0);
  assert(mean >= 10.0);
  assert(mean <= 20.0);
  assert(variance >= 0.0);
  assert(variance <= 50.0); // TODO: Use Popoviciu bound of 25.0
  

  return 0;
}
