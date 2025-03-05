// Insert a bug into the source code.
// Usage: brcont [C source] [index]

// Returns 0 if successful, 2 if all possibilities
// are exhausted.

#include <src/lex.h>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, __FILE__ ": ");
    fprintf(stderr, "Usage: %s <file> <index>\n", argv[0]) ;
    return 1;
  }    

  auto fobj = ReadAll(argv[1]);
  size_t idx = atol(argv[2]);
  auto tokens = Lex::CLangTokenize(fobj, true);

  // dump parser output for debugging
  auto root = Parser::CLangParser(tokens);
  bool ret = BugInsertor::MissingBreakOrCont(root, idx);

  if (!ret) {
    printf("/* cannot insert */\n");
    delete root;
    return 2;
  }

  root->Print(std::cout);
  delete root;
  return 0;
}
