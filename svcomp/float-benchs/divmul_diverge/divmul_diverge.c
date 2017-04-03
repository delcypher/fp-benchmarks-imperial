#include "svcomp/svcomp.h"
/* Example inspired from "The Arithmetic-Geometric Progression Abstract
   Domain" by Feret, published in VMCAI 05.
 */

void wait_for_clock() { }

int main()
{
  int i;
  float x;

  x = 0;

  for (i = 0; i < 3000000; i++) {

    if (__VERIFIER_nondet_int()) {
      x = __VERIFIER_nondet_float();
      // s/&&/&/ to avoid considering unnecessary paths
      __VERIFIER_assume(x >= -100.f & x <= 100.f);
    }

    x = x / 3.1f;
    x = x * 3.1f;

    wait_for_clock();
  }

  __VERIFIER_assert(x >= -1000.f && x <= 1000.f);
  return 0;
}
