// Print the name of variables in a tree structure.
// Usage: vartree [C source file]

#include <src/lex.h>
#include <iostream>
#include <unordered_set>

template <typename T>
void VecRemoveLast(std::vector<T> &vec) {
  if (vec.empty()) {
    return;
  }

  typename std::vector<T>::iterator it = vec.end();
  it--;
  vec.erase(it);
}

namespace Parser {

struct VarTable {
 public:
  VarTable() = default;

  void Enter() {
    table_.push_back({});
  }

  void Leave() {
    assert(!this->table_.empty());
    VecRemoveLast(this->table_);
  }

  auto Query(const std::string &var) const -> bool {
    for (const auto &tb : this->table_) {
      if (tb.find(var) != tb.end()) {
        return true;
      }
    }
    return false;
  }

  void Add(const std::string &var) {
    this->table_.back().insert(var);
  }

 private:
  std::vector<std::unordered_set<std::string> > table_;
};

static VarTable var_table;

static size_t indent = 0;

static auto PrintIndent(std::ostream &os) -> std::ostream & {
  for (size_t i = 1; i < indent; i++)
    os << ' ';
  return os;
}

static auto PrintVars(BasicBlock *root, std::ostream &os) 
  -> std::ostream & {
  
  {
    const auto &instr = root->GetInstrAsRef();
    const auto vars = instr.GetVarNames();

    for (const auto &var: vars) {
      PrintIndent(os);

      if (var_table.Query(var)) {

      } else {
        os << var << std::endl;
        var_table.Add(var);
      }
    }
  }

  indent ++;
  var_table.Enter();

  size_t num_children = root->GetNumChildren();
  for (size_t i = 0; i < num_children; i++) {
    PrintVars(root->GetChild(i), os);
  }

  // end of this block.
  var_table.Leave();
  indent --;

  return os;
}

} // namespace Parser

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: fntree [C source]\n");
    return 1;
  }

  auto fobj = ReadAll(argv[1]);
  auto tokens = Lex::CLangTokenize(fobj, true);

  // dump parser output for debugging
  auto root = Parser::CLangParser(tokens);
  PrintVars(root, std::cout);

  std::cout << std::endl;
  delete root;
  return 0;
}
