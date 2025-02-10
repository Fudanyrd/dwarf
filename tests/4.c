

char buf[32];
void _start() {
  char *pt;
  pt = &buf;

  // 4865 6c6c 6f20 776f 726c 6421 0a 
  // Hello world!
  *pt = 0x48;
  pt++;
  *pt = 0x65;
  pt++;
  *pt = 0x6c;
  pt++;
  *pt = 0x6c;
  pt++;
  *pt = 0x6f;
  pt++;
  *pt = 0x20;
  pt++;
  *pt = 0x77;
  pt++;
  *pt = 0x6f;
  pt++;
  *pt = 0x72;
  pt++;
  *pt = 0x6c;
  pt++;
  *pt = 0x64;
  pt++;
  *pt = 0x21;
  pt++;
  *pt = 0x0a;
  pt++;

  pt = &buf;
  write(1, pt, 15);
  exit(0);
}
