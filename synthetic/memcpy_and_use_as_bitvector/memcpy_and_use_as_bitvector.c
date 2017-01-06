#include "klee/klee.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int sink = 0;

int main(int argc, char** argv) {
  float f;
  klee_make_symbolic(&f, sizeof(float), "f");
  assert(sizeof(int) == sizeof(float));

  // Do some operation that means that f is more
  // than just a read of some bytes. It now is
  // an expression of type float.
  f = f + 1;

  int x = 0;
  memcpy(&x, &f, sizeof(float));

  // Now do some bitvector computation on x.
  int action = 0;
  klee_make_symbolic(&action, sizeof(int), "action");
  if (action < 0 || action > 100) {
    return 0;
  }
  int newValue = 0;
  unsigned int newValueUnsigned = 0;
  switch (action) {
    case 0: // +
      newValue = x + 1;
      printf("add\n");
      break;
    case 1: {
      // signed sub
      // Don't use constants here to avoid canonicalizing as AddExpr
      int havoc = 0;
      klee_make_symbolic(&havoc, sizeof(int), "havoc0");
      newValue = x - havoc;
      printf("signed sub\n");
      break;
    }
    case 2: {
      // unsigned sub
      // Don't use constants here to avoid canonicalizing as AddExpr
      unsigned int havoc = 0;
      klee_make_symbolic(&havoc, sizeof(unsigned int), "havoc1");
      newValue = ( (unsigned) x) - havoc;
      printf("unsigned sub\n");
      break;
    }
    case 3: // *
      newValue = x * 3;
      printf("multiply\n");
      break;
    case 4: // Unsigned divide
      newValueUnsigned = ((unsigned int) x) / 3;
      printf("signed divide\n");
      break;
    case 5: // Signed divide
      newValue = x / 3;
      printf("signed divide\n");
      break;
    case 6: // Unsigned
      newValueUnsigned = ((unsigned int) x) % 3;
      printf("unsigned modulo\n");
      break;
    case 7: // Signed modulo
      newValue = x % 3;
      printf("signed modulo\n");
      break;
    case 8: // &
      newValue = x & 16;
      printf("and\n");
      break;
    case 9: // |
      newValue = x | 13;
      printf("or\n");
      break;
    case 10: // ^
      newValue = x ^ 13;
      printf("xor\n");
      break;
    case 11:
      // Bitwise not
      newValue = ~x;
      printf("bitwise not\n");
      break;
    case 12:
      // << by constant
      newValue = x << 3;
      printf("<< by constant\n");
      break;
    case 13: {
      // << by symbolic
      unsigned int shiftAmount = klee_range(0, 32, "shiftAmount");
      newValue = x << shiftAmount;
      printf("<< by symbolic\n");
      break;
    }
    case 14:
      // Arithmetic right shift by constant
      newValue = x >> 3;
      printf("Arithmetic right shift by constant\n");
      break;
    case 15: {
      // Arithmetic right shift by symbolic
      unsigned int shiftAmount = klee_range(0, 32, "shiftAmount1");
      newValue = x >> shiftAmount;
      printf("Arithmetic right shift by symbolic\n");
      break;
    }
    case 16:
      // Logical right shift by constant
      newValueUnsigned = ((unsigned int) x) >> 3;
      printf("Logical right shift by constant\n");
      break;
    case 17: {
      // Logical right shift by symbolic
      unsigned int shiftAmount = klee_range(0, 32, "shiftAmount1");
      newValueUnsigned = ((unsigned int) x) >> shiftAmount;
      printf("Logical right shift by symbolic\n");
      break;
    }
    case 18:
      // slt
      newValue = (x < 5);
      printf("slt\n");
      break;
    case 19:
      // sle
      newValue = (x <= 5);
      printf("sle\n");
      break;
    case 20:
      // sgt
      newValue = (x > 5);
      printf("sgt\n");
      break;
    case 21:
      // sge
      newValue = (x >= 5);
      printf("sge\n");
      break;
    case 22:
      // ult
      newValueUnsigned = ((unsigned int) x) < 5;
      printf("ult\n");
      break;
    case 23:
      // ule
      newValueUnsigned = ((unsigned int) x) <= 5;
      printf("ule\n");
      break;
    case 24:
      // ugt
      newValueUnsigned = ((unsigned int) x) > 5;
      printf("ugt\n");
      break;
    case 25:
      // uge
      newValueUnsigned = ((unsigned int) x) >= 5;
      printf("uge\n");
      break;
    case 26: {
      // SItoFP
      float dummy = (float) x;
      newValue = dummy > 0.0f;
      printf("SItoFP\n");
      break;
    }
    case 27: {
      // UItoFP
      float dummy = (float) ((unsigned int) x);
      newValueUnsigned = dummy > 0.0f;
      printf("UItoFP\n");
      break;
    }
    case 28: {
      // Sign extend
      int64_t extended = x;
      newValue = extended > 22;
      printf("sign extend\n");
      break;
    }
    case 29: {
      // zero extend
      uint64_t extended = (unsigned int) x;
      newValueUnsigned = extended > 22;
      printf("zero extend\n");
      break;
    }
    case 30: {
      // Use x as an index
      int havoc = 0;
      klee_make_symbolic(&havoc, sizeof(int), "havoc2");
      char a[2] = { (char) havoc, (char) havoc };
      // Testing that we can use x as an index.
      if (x < 1 && x >= 0) {
        newValue = a[x] > 5;
      }
      break;
    }
    case 31: {
      // ExtractExpr
      char small_value = 0;
      small_value = (int8_t) x;
      newValue = small_value > 0;
      break;
    }
    case 32: {
      // EqExpr
      int havoc = 0;
      klee_make_symbolic(&havoc, sizeof(int), "havoc3");
      newValue = (x == havoc);
      printf("EqExpr\n");
    }
    case 33: {
      // SelectExpr
      int havoc = 0;
      klee_make_symbolic(&havoc, sizeof(int), "havoc4");
      newValue = havoc ? x : 0;
      printf("SelectExpr\n");
    }
    default:
      break;
  }

  // Use expression in condition so that we force a call to the solver
  if (newValue > 0) {
    sink += 1;
  }

  if (newValueUnsigned > 5) {
    sink += 1;
  }

  // Prevent removal of unused values
  return sink;
}
