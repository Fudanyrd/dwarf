// if the compiler works correctly,
// the output of this program should be:
// "ABCDE"
char ch[16];

void _start() {
  char *pt;
  pt = &ch;
  *pt = 65;

  write(1, pt, 1);

  {
    char foo;
    char bar;

    foo = 66;
    pt = &foo;
    write(1, pt, 1);

    bar = 67;
    pt = &bar;
    write (1, pt, 1);
  }
  {
    char foo;
    char bar;

    foo = 68;
    pt = &foo;
    write (1, pt, 1);

    bar = 69;
    pt = &bar;
    write(1, pt, 1);
  }

  while (0) {
    // should not execute
    *pt = 66;
    write(1, pt, 1);
  }

  if (1) {
    *pt = 10; // '\n'
    write(1, pt, 1);
    // should execute
    exit(0);
  }
}
