int strlen(char *str) {
  int ret;
  ret = 0;
  char ch;
  ch = *str;

  bool end;
  end = ch != 0;
  while (end) {
    ret ++;
    str ++;
    ch = *str;
    end = ch != 0;
  }

  return ret;
}

void _start() {
  char *pt;
  pt = "Hello world!\n";

  int len;
  len = strlen(pt);
  write(1, pt, len);
  exit(0);
}
