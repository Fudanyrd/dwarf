// Parse a C source file and print the structure of
// the corresponding AST.
// Beta feature. PLEASE USE WITH CAUTION, AND REPORT BUG TO
// ITS AUTHOR.
// Usage: parse [c source file]

#include <src/lex.h>
#include <iostream>

int main(int argc, char ** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]) ;
    return 1;
  }

  auto fobj = ReadAll(argv[1]);
  auto tokens = Lex::CLangTokenize(fobj, true);

  // dump tokenizer output for debugging
  FILE *fout = fopen("tokens.csv", "w");
  assert( fout != nullptr );
  for (const auto &token : tokens) {
    fprintf(fout, "%s,%u,%s\n", EncodeString(token.buf).c_str(), token.line,
            GetNameOfLabel(token.label));
  }
  fclose(fout);
  // dump parser output for debugging
  auto root = Parser::CLangParser(tokens);
  root->Print(std::cout);

  delete root;
  return 0;
}

