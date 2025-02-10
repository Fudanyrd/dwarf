#include <src/lex.h>
#include <src/utils.h>

#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
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

  auto generator = Generator::X86Generator();
  auto asm_code = generator.GenerateCode(root);
  std::ofstream asm_out("test.S");
  asm_out << asm_code;
  asm_out.close();

  delete root;
  return 0;
}
