// check that the compiler handles while loop
// and function call. If the compiler works,
// The output of this program should be:
// "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`"

void putchar(char ch) {
  char *pt;
  pt = &ch;
  write(1, pt, 1);
  return;
}

void _start() {
  int ch;
  ch = 65;
  
  bool end;
  end = ch != 97;
  while (end) {
    putchar(ch);
    ch = ch + 1;
    end = ch != 97;
  }

  putchar(10);
  exit(0);
}
