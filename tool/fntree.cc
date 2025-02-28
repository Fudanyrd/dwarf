// Print the function calls in a C source file
// in a tree structure.

// Usage: fntree [C source file]

// Example output:
// _strlen
// _start
//   _strlen
//   write
//   exit

#include <src/lex.h>
#include <iostream>

namespace Parser {

static size_t indent = 0;

static auto PrintIndent(std::ostream &os) -> std::ostream & {
  for (size_t i = 1; i < indent; i++)
    os << '\t';
  return os;
}

static auto PrintFuncCall(BasicBlock *root, std::ostream &os) 
  -> std::ostream & {
  
  {
    const auto &instr = root->GetInstrAsRef();
    const auto fns = instr.GetFuncCalls();

    for (const auto &fn : fns) {
      PrintIndent(os);
      os << fn << std::endl;
    }
  }

  indent ++;
  size_t num_children = root->GetNumChildren();
  for (size_t i = 0; i < num_children; i++) {
    PrintFuncCall(root->GetChild(i), os);
  }
  indent --;

  return os;
}

}

int main(int argc, char **argv, char **envp) {
  if (argc < 2) {
    fprintf(stderr, "Usage: fntree [C source]\n");
    return 1;
  }

  auto fobj = ReadAll(argv[1]);
  auto tokens = Lex::CLangTokenize(fobj, true);

  // dump parser output for debugging
  auto root = Parser::CLangParser(tokens);
  Parser::PrintFuncCall(root, std::cout);

  delete root;
  return 0;
}
