#ifdef KLEE
#include "klee/klee.h"
#endif
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_histogram2d.h>

#include <assert.h>

int main (void)
{
  const gsl_rng_type * T;
  gsl_rng * r;

  gsl_histogram2d * h = gsl_histogram2d_alloc (10, 10);

  gsl_histogram2d_set_ranges_uniform (h, 
                                      0.0, 1.0,
                                      0.0, 1.0);

  double expected[10][2] = { {0.89997, 0.116291}, 
			     {0.816784, 0.19472},
			     {0.810872, 0.148497},
			     {0.895067, 0.174431},
			     {0.846645, 0.173995},
			     {0.872153, 0.165864},
			     {0.820614, 0.18044},
			     {0.844282, 0.116857},
			     {0.839161, 0.139231},
			     {0.809713, 0.121319} };


  float x1 = 0.3, y1 = 0.3, h1 = 0.3;
#if KLEE
  klee_make_symbolic(&x1, sizeof(x1), "x1");
  klee_assume(x1 > 0.2);
  klee_assume(x1 < 0.4);
  //klee_make_symbolic(&y1, sizeof(y1), "y1");
  //klee_make_symbolic(&h1, sizeof(h1), "h1");
  //klee_assume(h1 > 0);
#endif

  gsl_histogram2d_accumulate (h, x1, y1, h1);
  gsl_histogram2d_accumulate (h, 0.8, 0.1, 5);
  gsl_histogram2d_accumulate (h, 0.7, 0.9, 0.5);

  gsl_rng_env_setup ();
  
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  {
    int i;
    gsl_histogram2d_pdf * p 
      = gsl_histogram2d_pdf_alloc (h->nx, h->ny);
    
    gsl_histogram2d_pdf_init (p, h);

    for (i = 0; i < 10; i++) {
      double x, y;
      double u = gsl_rng_uniform (r);
      double v = gsl_rng_uniform (r);
       
      gsl_histogram2d_pdf_sample (p, u, v, &x, &y);
      
#ifndef KLEE
      printf ("%g %g\n", x, y);
#endif
      
#ifdef BUG
      assert(x != expected[i][0]);
      assert(y != expected[i][1]);
#endif

    }

    gsl_histogram2d_pdf_free (p);
  }

  gsl_histogram2d_free (h);
  gsl_rng_free (r);

  return 0;
}
