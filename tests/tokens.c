// check the functionality of the tokenizer
// NOTE: a lot of corner cases are covered.
#include <foo>

enum {};

int main() {
  unsigned long i = 0;
  signed int j = 1;
  int *pt = 0, "foo", 'b';

  struct some {
    int i;
    char j;
  } any;
  /* block comment */

  union other {
    // line comment
    short i;
  };

  for (;0;) {
    pt++;
    pt--;
    i + 1;
    j - 1;
    j <= i;
    j >= i;
    j < i;
    j > i;
    j = i;
    pt.j;
    pt ->i;
    continue;
  }
  while (0) {
    pt[2];
    break;
  }

  do {
    switch (0) {
    case (1 != 1):
    case (1 == 1 ? 1 : 0):
    default:
    }
  } while (0);

  return 0;
}
