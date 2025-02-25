#ifndef __LEX_H__
#define __LEX_H__

#define TO_STD_STRING(x) x

#include "utils.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <memory>
#include <stack>
#include <sstream>
#include <queue>

namespace Lex {

enum class TokenLabel {
  TNULL = 0, // comments, whitespaces, etc.
  TALPHA, // isalpha or isdigit
  TDIGIT, // is digit
  TQUOTE, // single quote, '
  TDOUBLEQUOTE, // double quote, "
  TLEFTBRACKET, // {
  TRIGHTBRACKET, // }
  TLEFTSQ, // [
  TRIGHTSQ, // ]
  TLEFTPARENT, // (
  TRIGHTPARENT, //)
  TSEMICOLON, // ;
  TOPERATOR, // one of !@#$%^&*+=/|
  // operators
  TADD, // +
  TINCR, // ++
  TSUB, // -
  TDECR, // --
  TMUL, // *, also deref
  TDIV, // /
  TREM, // %
  TADRP, // &
  TDOT, // .
  TARROW, // ->
  TGE, // >
  TLE, // <
  TGEQ, // >=
  TLEQ, // <=
  TEQ, // ==
  TNE, // !=
  TASSIGN, // =
  TNOT, // !
  TCOMMA, // ,
  TQUESTION, // ?
  TCOLON, // :
  TSHARP, // #
  // supported types: bool, int, char, void
  TBOOL,
  TINT,
  TCHAR,
  TVOID,
  TLONG,
  TSIGNED,
  TUNSIGNED,
  TSHORT,
  // supported control flow keywords: if, else, while, return
  TIF,
  TELSE,
  TWHILE,
  TRETURN,
  TFOR,
  TDO,
  TSWITCH,
  TCASE,
  TDEFAULT,
  TBREAK,
  TCONTINUE,
  // struct, union, enum
  TSTRUCT,
  TUNION,
  TENUM,
};

const char *GetNameOfLabel(TokenLabel label);

struct Token {
 public:
  std::string buf; // string buffer
  TokenLabel label; // token label
  uint32_t line;   // line number

  // default constructor
  Token(): label(TokenLabel::TNULL), line(0) {}
  Token(const std::string &word, TokenLabel lbl, uint32_t lno)
    : buf(word), label(lbl), line(lno) {}

  auto operator=(const Token &token) -> Token & = default;

  auto Print(std::ostream &os) const -> std::ostream & {
    os << this->buf;
    return os;
  }

  // destructor
  ~Token() = default;
};

// Tokenizer for C language
auto CLangTokenize(const std::string &fobj, bool IgnoreNull) -> std::vector<Token>;

// Remove null tokens, and clear the vector
auto RemoveNullTokens(std::vector<Token> &tokens) -> std::vector<Token>;

} // namespace Lex

namespace Parser {

static const size_t max_recursion = 256;

struct Instruction {
 public:
  std::vector<Lex::Token> tokens;

  Instruction() = default;
  ~Instruction() = default;

  auto Print(std::ostream &os) const -> std::ostream &;

  auto GetTypeOfToken(size_t idx) const -> Lex::TokenLabel {
    return idx < tokens.size() ? tokens[idx].label : Lex::TokenLabel::TNULL;
  }
};

enum class BlockType {
  BCOMMON = 0, 
  BFUNCTION,
  BIF,
  BELSE,
  BWHILE,
  BVARDECLARE,
  BRET,
  BIFELSE,
};

auto BlockTypeToString(BlockType bt) -> std::string;

class BasicBlock {
 public:
  BasicBlock() = default;
  BasicBlock(const Instruction &instruction): instruction(instruction), children({}) {}

  ~BasicBlock() {
    for (auto *child : children) {
      delete child;
    }
  }

  void AddChild(BasicBlock *child) { children.push_back(child); }

  auto GetType(void) const -> BlockType { return this->btype; }
  // do not use this function
  auto SetType(BlockType btype) -> void { this->btype = btype; }

  auto GetInstruction(void) const -> Instruction { return this->instruction; }

  auto GetInstrAsRef(void) const -> const Instruction & { return this->instruction; }

  auto GetChild(size_t idx) const -> BasicBlock * { return children[idx]; }

  auto GetNumChildren(void) const -> size_t { return children.size(); }
  auto HasChildren(void) const -> bool { return !children.empty(); }

  auto Print(std::ostream &os) const -> std::ostream &;

  // helper method. Do not use these
  static void ReshapeBlock(BasicBlock *root);
  static void ReshapeBlockTree(BasicBlock *root);
  static void MergeIfElseBlock(BasicBlock *root);
  static void MergeIfElseBlockTree(BasicBlock *root);

 private:
  BlockType btype{BlockType::BCOMMON};
  Instruction instruction;
  std::vector<BasicBlock *> children;

};

// parser for c language
auto CLangParser(const std::vector<Lex::Token> &tokens) -> BasicBlock *;

} // namespace Parser

namespace Generator {

struct StackFrame {
 public:
  // the stack pointer when the function is called
  size_t initial_sp{0};
  // allocated size
  size_t alloc_size{0};

  StackFrame() = default;
  ~StackFrame() = default;

  // disallow copy
  StackFrame(const StackFrame &) = delete;
  StackFrame &operator=(const StackFrame &) = delete;

  auto Alloc(size_t size) -> size_t;

 private:
  std::queue<size_t> free_list_of_byte; // 1 byte
  std::queue<size_t> free_list_of_word; // 4 byte
  std::queue<size_t> free_list_of_dword; // 8 bytes

  auto AllocDoubleWord() -> size_t;
  auto AllocWord() -> size_t;
  auto AllocByte() -> size_t;
};

struct SymbolType {
 public:
  uint32_t pointer_level{0};
  enum BaseType {
    TVOID = 0,
    TINT,
    TBOOL,
    TCHAR,
    TFUNCTION,
  } base_type;
  // if it is not an array, then it's an scalar.
  bool is_array{false};
  bool is_global{false};
  // size of the fixed-length array
  size_t array_size{0};
  // address of the symbol is (initial sp - addr)
  size_t addr{0};
  std::shared_ptr<StackFrame> stack_frame;

  SymbolType() = default;
  ~SymbolType() = default;

  SymbolType &operator=(const SymbolType &other) = default;

  SymbolType(uint32_t ptr_level, enum SymbolType::BaseType base_type, bool is_array)
      : pointer_level(ptr_level), base_type(base_type), is_array(is_array) {
    
    if (base_type == SymbolType::BaseType::TVOID && ptr_level == 0) {
      // cannot create scalar of void type.
      fprintf(stderr, "Cannot create scalar of void type\n");
      assert(false);
    } 
  }

  auto MemorySize() const -> size_t {
    size_t scalar_size = 0;
    if (pointer_level) {
      scalar_size = sizeof(void *);
    } else {
      switch (this->base_type) {
      case SymbolType::BaseType::TBOOL:
      case SymbolType::BaseType::TCHAR: {
        scalar_size = sizeof(char);
        break;
      }
      case (SymbolType::BaseType::TINT): {
        scalar_size = sizeof(int);
        break;
      }
      default: break;
      }
    }

    assert(scalar_size > 0);
    size_t ret = is_array ? scalar_size * array_size : scalar_size;
    if (ret == 1) {
      // simply return
      return ret;
    }
    // round up to a multiple of 4 bytes
    return (ret + 3) & (~3);
  }

  auto GetAddr() const -> size_t {
    if (this->is_global) {
      // manually lookup in the .bss section
      // in this case, the address should be
      // var_name(%rip)
      return 0;
    }

    // the address should be
    // offset(%rsp)
    assert(this->stack_frame != nullptr);
    return this->stack_frame->initial_sp + this->addr;
  }
};

// 16MB stack size
constexpr size_t max_stack_size = (16 * 1024 * 1024);

class SymbolTable {
 public:
  SymbolTable() = default;
  ~SymbolTable()  = default;
  SymbolTable &operator=(const SymbolTable &) = delete;

  auto Lookup(const std::string &name) -> SymbolType *;
  
  // add a symbol to the table, and allocate memory on 
  // stack for it.
  void AddSymbol(const std::string &name, SymbolType type) {
    auto &it = this->table_stack.back();
    if (it.find(name) != it.end()) {
      throw std::runtime_error("Symbol already exists");
    }
    assert(type.addr != 0 || type.is_global);
    it[name] = type;
  }

  auto Enter(std::ostringstream &os) -> std::ostringstream & { 
    assert(this->table_stack.size() <= Parser::max_recursion);

    // push a stack frame to the stack.
    size_t current_sp = 0;
    if (!this->stack_frames.empty()) {
      auto frame_ptr = this->stack_frames.top();
      current_sp = frame_ptr->initial_sp + frame_ptr->alloc_size;
    }
    assert(current_sp < max_stack_size);
    std::shared_ptr<StackFrame> current_frame = std::make_shared<StackFrame>();
    current_frame->initial_sp = current_sp;
    current_frame->alloc_size = 0;
    this->table_stack.push_back({}); 
    this->stack_frames.push(current_frame);

    return os;
  }

  auto Leave(std::ostringstream &os) -> std::ostringstream & { 
    assert(!this->stack_frames.empty());
    assert(!this->table_stack.empty());
    auto last = this->table_stack.end();
    last--;
    this->table_stack.erase(last);

    auto frame_ptr = this->stack_frames.top();
    // recover the stack pointer
    if (frame_ptr->alloc_size != 0) {
      // example: addq $16, %rsp
      os << TO_STD_STRING("\taddq $"); 
      os << frame_ptr->alloc_size ;
      os << TO_STD_STRING(", %rsp\n");
    }
    this->stack_frames.pop();
    return os;
  }

  // returns number of stack frames
  auto GetStackDepth() const -> size_t { return this->table_stack.size(); }

  // returns stack size in bytes
  auto GetStackSize() const -> size_t {
    if (this->stack_frames.empty()) {
      return 0;
    }

    auto top = stack_frames.top();
    auto ret = top->alloc_size + top->initial_sp;
    assert(ret <= max_stack_size);
    return ret;
  }

  auto GetCurrentStackFrame() -> std::shared_ptr<StackFrame> { return this->stack_frames.top(); }

 private:
  typedef std::unordered_map<std::string, SymbolType> Table;
  std::vector<Table> table_stack;
  std::stack<std::shared_ptr<StackFrame>> stack_frames;
};

// Base class of code generator
class CodeGenerator {
 public:
  virtual auto GenerateCode(Parser::BasicBlock *root) -> std::string {
    throw std::runtime_error("Not implemented");
  }

  virtual auto GenerateCodeWithDebugInfo(Parser::BasicBlock *root) -> std::string {
    throw std::runtime_error("Not implemented");
  }

  virtual ~CodeGenerator() = default;
 protected:
  SymbolTable symtab;
  size_t branch_count{0};
};

class X86Generator : public CodeGenerator {
 public:
  X86Generator() = default;
  auto GenerateCode(Parser::BasicBlock *root) -> std::string override;
  auto GenerateCodeWithDebugInfo(Parser::BasicBlock *root) -> std::string override;

 private:
  enum class X86Registers {
    AX = 0,
    BX,
    CX,
    DX,
    SI, // 4
    DI,
    BP,
    SP,
    R8,
    R9, // 9
    R10,
    R11,
    R12,
    R13,
    R14, // 14
    R15,
  };

  // use this to store c strings.
  // example {"hello", 2}
  std::unordered_map<std::string, size_t> c_strs;
  size_t c_str_count{0};

  auto GetNameOfStringByIdx(size_t idx) -> std::string {
    return ".LC" + std::to_string(idx);
  }

  auto GetNameOfString(const std::string &str) -> std::string {
    assert(str.front() == '\"' && str.back() == '\"' 
           && str.size() >= 2);
    
    auto it = this->c_strs.find(str);
    if (it != this->c_strs.end()) {
      return GetNameOfStringByIdx(it->second);
    }
    // allocate an name for it.
    size_t idx = this->c_str_count++;
    this->c_strs[str] = idx;
    return GetNameOfStringByIdx(idx);
  }

  auto DumpCString(std::ostringstream &os) -> std::ostringstream & {
    // example:
    // .section .rodata
    // .LC0:
    // .string "hello world"
    // these strings are named .LC0, .LC1, etc.

    for (const auto &pair : c_strs) {
      const std::string name = this->GetNameOfStringByIdx(pair.second);
      os << "\t.section .rodata\n";
      os << name << ":\n";
      os << "\t.string " << pair.first << "\n";
    }

    return os;
  }

  auto GenerateCodeForInstruction(std::ostringstream &oss, const 
    Parser::Instruction &instr) -> std::ostringstream &;
  
  auto GenerateCodeForBlock(std::ostringstream &oss, Parser::BasicBlock *block) 
    -> std::ostringstream &;
  
  auto LoadVarIntoReg(std::ostringstream &oss, const std::string &var_name, X86Registers reg)
    -> std::ostringstream &;
  
  auto StoreVarFromReg(std::ostringstream &oss, const std::string &var_name, X86Registers reg)
    -> std::ostringstream &;

  /* Load a value(can be a variable or number) into a register */
  auto LoadValueIntoReg(std::ostringstream &oss, const Lex::Token &token, X86Registers reg)
    -> std::ostringstream &;

  // Store arguments from register into memory,
  // and add them to symbol table.
  // FIXME: cannot handle array-type arguments
  auto StoreArgsIntoMem(std::ostringstream &oss, const Parser::Instruction &declaration)
    -> std::ostringstream &;  
};

} // namespace Generator

namespace BugInsertor {

} // namespace BugInsertor

#endif // __LEX_H__
