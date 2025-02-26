// Print a particular function in a C source file.

// Usage
// funccopy [C source] [index]
// Where index is an index in decimal.

// Return value
// 0 if success, 2 if index is out of bound.

// The output format
//  The first row of output is simply two unsigned integer
//  in decimal, which is the start line and end line of a function.
//  
//  The lines that follow are the code of the function.

#include <iostream>
#include <src/lex.h>
#include <cstdlib>

int main(int argc, char **argv, char **envp) {
  if (argc < 3) {
    fprintf(stderr, __FILE__ ": ");
    fprintf(stderr, "Usage: %s <file> <index>\n", argv[0]) ;
    return 1;
  }

  auto fobj = ReadAll(argv[1]);
  size_t idx = atol(argv[2]);
  int ret = 0;
  auto tokens = Lex::CLangTokenize(fobj, true);

  // dump parser output for debugging
  auto root = Parser::CLangParser(tokens);
  size_t num_children = root->GetNumChildren();

  std::vector<Parser::BasicBlock *> funcs;

  for (size_t i = 0; i < num_children; i++) {
    auto *child = root->GetChild(i);
    if (child->GetType() == Parser::BlockType::BFUNCTION) {
      funcs.push_back(child);
    }
  }

  if (idx >= funcs.size()) {
    printf("0 0\n(null)\n");
    ret = 2;
  } else {
    auto *func = funcs[idx];
    auto rg = func->GetLineRange();
    printf("%lu %lu\n", rg.first, rg.second);
    func->Print(std::cout);
    std::cout << std::endl;
  }

  delete root;
  return ret;
}
