#include "svcomp/svcomp.h"

#if FLOAT_TYPE == float
#define FLOAT_CONSTANT(X) X ## f

#if EXACT
#define START_VALUE FLOAT_CONSTANT(1e7)
#define RESULT_VALUE FLOAT_CONSTANT(2.0)
#else
#define START_VALUE FLOAT_CONSTANT(1e8)
#define RESULT_VALUE FLOAT_CONSTANT(0.0)
#endif

#elif FLOAT_TYPE == double
#define FLOAT_CONSTANT(X) X

#define START_VALUE FLOAT_CONSTANT(1e8)
#define RESULT_VALUE FLOAT_CONSTANT(2.0)

#if EXACT
#error EXACT should be false for FLOAT_TYPE double
#endif

#else
#error Invalid FLOAT_TYPE
#endif

int main(int argc, char** argv)
{
  FLOAT_TYPE x, y, z, r;
  x = START_VALUE;
  y = x + FLOAT_CONSTANT(1.0);
  z = x - FLOAT_CONSTANT(1.0);
  r = y - z;
  __VERIFIER_assert(r == RESULT_VALUE);
  return 0;
}
