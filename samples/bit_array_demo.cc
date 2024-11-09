
#include <assert.h>

#include <iostream>

#include "bit_array.h"

int main() {
  bit_array x = 3, y = 5, z = 7;

  assert(x[0]);
  assert(x[1]);
  assert(!x[2]);
  assert(!x[100]);

  assert(y[0]);
  assert(!y[1]);
  assert(y[2]);
  assert(!y[100]);

  assert(z[0]);
  assert(z[1]);
  assert(z[2]);
  assert(!z[100]);

  x.set_bit(3);
  assert(x[3]);
  assert(x == 11);
  x.clear_bit(1);
  assert(!x[1]);
  assert(x == 9);

  assert(x == x);

  x = 3;

  x.set_bit(10000);
  assert(x[10000]);
  assert(!x[9999]);
  assert(!x[100001]);
  x.clear_bit(10000);
  assert(x == 3);

  x = 3;

  assert(!x.lies_below(y));
  assert(x.lies_below(z));

  assert(!y.lies_over(x));
  assert(y.lies_below(z));

  assert(z.lies_over(x));
  assert(z.lies_over(y));

  x.set_bit(10000);
  assert(!x.lies_below(z));
  assert(!z.lies_over(x));

  z.set_bit(10000);
  assert(x.lies_below(z));
  assert(z.lies_over(x));

  x.clear_bit(10000);
  assert(x.lies_below(z));
  assert(z.lies_over(x));

  z.clear_bit(10000);
  assert(x.lies_below(z));
  assert(z.lies_over(x));

  x.clear();
  assert(x == 0);

  return 0;
}
