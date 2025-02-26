// List all function names in a C source file.
// Usage: funcs [C source file]

// The output is in CSV format:
// function_name, start_line, end_line

#include <iostream>
#include <src/lex.h>

int main(int argc, char **argv, char **envp) {
  if (argc < 2) {
    fprintf(stderr, __FILE__ ": ");
    fprintf(stderr, "Usage: %s <file>\n", argv[0]) ;
    return 1;
  }

  auto fobj = ReadAll(argv[1]);
  auto tokens = Lex::CLangTokenize(fobj, true);

  // dump parser output for debugging
  auto root = Parser::CLangParser(tokens);
  size_t num_children = root->GetNumChildren();

  for (size_t i = 0; i < num_children; i++) {
    auto *child = root->GetChild(i);
    if (child->GetType() == Parser::BlockType::BFUNCTION) {
      auto rg = child->GetLineRange();
      const auto insn = child->GetInstruction();
      insn.Print(std::cout);
      printf(", %lu, %lu\n",  rg.first, rg.second);
    }
  }

  delete root;
  return 0;
}
