#ifdef KLEE
#include "klee/klee.h"
#endif

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <gsl/gsl_integration.h>

double f (double x, void * params) {
  double alpha = *(double *) params;
  double f = log(alpha*x) / sqrt(x);
  return f;
}

int
main (void)
{
  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);
  
  double result, error;
  double expected = -4.0;
  double alpha = 1.0;

  gsl_function F;
  F.function = &f;
  F.params = &alpha;

  double a = 0;
  double b = 1;

  gsl_integration_qags (&F, a, b, 0, 1e-7, 1000,
                        w, &result, &error); 

#ifdef KLEE
  klee_make_symbolic(&b, sizeof(b), "b");
#endif

#ifndef KLEE
  printf ("result          = % .18f\n", result);
  printf ("exact result    = % .18f\n", expected);
  printf ("estimated error = % .18f\n", error);
  printf ("actual error    = % .18f\n", result - expected);
  printf ("intervals       = %zu\n", w->size);
#endif

#ifdef BUG
  assert(result - expected > 0.00000001);
#endif

  gsl_integration_workspace_free (w);

  return 0;
}
