#ifdef KLEE
#include "klee/klee.h"
#endif

#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

double f (double x, void * params)
{
  (void)(params); /* avoid unused parameter warning */
  return pow (x, 1.5);
}

int
main (int argc, char** argv)
{
  gsl_function F;
  double result, abserr;

  F.function = &f;
  F.params = 0;

#ifndef KLEE
  printf ("f(x) = x^(3/2)\n");
#endif

  double x1 = 2.0;
  double h1 = 1e-8;
#ifdef KLEE
  klee_make_symbolic(&h1, sizeof(double), "h1");
  klee_assume(h1 >= 1e-8);
  klee_assume(h1 <= 1e-8 + 1e-10);
#endif

  gsl_deriv_central (&F, x1, h1, &result, &abserr);

#ifndef KLEE
  printf ("x = 2.0\n");
  printf ("f'(x) = %.10f +/- %.10f\n", result, abserr);
  printf ("exact = %.10f\n\n", 1.5 * sqrt(2.0));
#endif

  double x2 = 0.0; 

#ifdef KLEE
  klee_make_symbolic(&x2, sizeof(double), "x2");
  klee_assume(x2 >= 0.0);
  klee_assume(x2 <= 1e-16);
#endif

  gsl_deriv_forward (&F, x2, 1e-8, &result, &abserr);

#ifndef KLEE
  printf ("x = 0.0\n");
  printf ("f'(x) = %.10f +/- %.10f\n", result, abserr);
  printf ("exact = %.10f\n", 0.0);
#endif

  return 0;
}
