// check the functionality of the tokenizer
// NOTE: a lot of corner cases are covered.
#include <foo>

#define FOO do { \
    abort(); \
  } while (0)

enum {
  none = 0,
};

static char *about_the_author = "I\'m the author of this \
dwarf project. My name is @Fudanyrd. \
Glad to meet you!.";

/* Check for operators, like +, ++, +=, etc. */
int and(int x, int y) {
  int ret = x & y;
  ret = x; 
  ret &= y;
  return ret;
}

int or(int x, int y) {
  int ret = x | y;
  ret = x; 
  ret |= y;
  return ret;
}

int xor(int x, int y) {
  int ret = x ^ y;
  ret = x; 
  ret ^= y;
  return ret;
}

int add(int x, int y) {
  int ret = x;
  ret += y;
  ret ++; ret--;
  return ret;
}

int mult(int x, int y) {
  int ret = x * y;
  x *= y;
  return ret;
}
int div(int x, int y) {
  int ret = x / y;
  x /= y;
  return ret;
}

int sub(int x, int y) {
  int ret = x;
  x -= y;
  return ret;
}

static char buf[12];
extern int count;

int main(int argc, char **argv) {
  unsigned long i = 0;
  signed int j = 1;
  int *pt = 0, "f\'o\"o", '\\b';

  i = i && j;
  j = ~j;
  i = i || j;
  j = j & i;
  i = i | j;

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
