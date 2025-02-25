// Tokenize a given C source file.
// The output file is tokens.csv
// Usage: tokenize [c source file]
#include <src/lex.h>
#include <src/utils.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]) ;
    return 1;
  }

  auto fobj = ReadAll(argv[1]);
  auto tokens = Lex::CLangTokenize(fobj, false);

  // dump tokenizer output for debugging
  FILE *fout = fopen("tokens.csv", "w");
  assert( fout != nullptr );
  for (const auto &token : tokens) {
    fprintf(fout, "%s,%u,%s\n", EncodeString(token.buf).c_str(), token.line,
            GetNameOfLabel(token.label));
  }
  fclose(fout);

  return 0;
}
