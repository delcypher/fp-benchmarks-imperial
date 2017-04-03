#include "svcomp/svcomp.h"
/* Example from "The ASTRÉE Analyzer" by Cousot, Cousot, Feret, Mauborgne,
   Miné, Monniaux, and Rival, published in ESOP 05.
 */
void wait_for_clock() { }

int main()
{
  float A, B, X;
  int i;

  A = 0.f;
  B = 0.f;

  for (i = 0; i < 3600000; i++) {

    if (__VERIFIER_nondet_int()) {
      X = __VERIFIER_nondet_float();
      // s/&&/&/ to avoid considering unnecessary paths
      __VERIFIER_assume(X >= -20.f & X <= 20.f);
    }
    else {
      X = B;
    }
    B = B - (2.f*B - A - X) * 0.005f;
    A = X;

    wait_for_clock();
  }

  __VERIFIER_assert(A >= -100.f && A <= 100.f);
  return 0;
}
