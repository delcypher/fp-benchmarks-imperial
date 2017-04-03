#include "svcomp/svcomp.h"
/* Inverse square root computation by Newton's method, from Quake sources.
   See also "Fast Inverse Square root" by Chris Lomont.
*/
float InvSqrt(float x)
{
  float xhalf = 0.5f*x;
  int i = *(int*)&x;
  i = 0x5f3759df - (i>>1);
  x = *(float*)&i;
  x = x*(1.5f-xhalf*x*x);
  return x;
}

int main()
{
  float a,r;

  a = __VERIFIER_nondet_float();
  // s/&&/&/ to avoid considering unnecessary paths
  __VERIFIER_assume(a >= 0.1f & a <= 100.f);

  r = InvSqrt(a);

  __VERIFIER_assert(r >= 0.f && r <= 10.);
  return 0;
}
