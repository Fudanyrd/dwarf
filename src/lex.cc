#include "lex.h"
#include "utils.h"
#include <cassert>
#include <cstdint>
#include <sstream>
#include <stack>
#include <utility>
#include <cctype>

namespace Lex {

// line number
static uint32_t lno = 0;

static const char *token_names[] = {
  "null",
  "alpha",
  "digit",
  "quote",
  "double quote",
  "left bracket",
  "right bracket",
  "left square bracket",
  "right square bracket",
  "left parent",
  "right parent",
  "semicolon",
  "operator",
  // operators
  "add",
  "incr",
  "sub",
  "decr",
  "mult",
  "div",
  "rem",
  "adrp",
  "dot",
  "arrow",
  "ge",
  "le",
  "geq",
  "leq",
  "eq",
  "ne",
  "assign",
  "not",
  "comma",
  "question",
  "colon",
  "sharp",
  "and",
  "pipe",
  "or",
  "flip",
  "xor",
  "add=",
  "sub=",
  "mult=",
  "div=",
  "rem=",
  "and=",
  "xor=",
  "or=",
  // supported types: bool, int, char, void
  "bool",
  "int",
  "char",
  "void",
  "long",
  "signed",
  "unsigned",
  "short",
  // supported control flow keywords: if, else, while, return
  // , for, do, case, break, continue
  "if",
  "else",
  "while",
  "return",
  "for",
  "do",
  "switch",
  "case",
  "default",
  "break",
  "continue",
  // struct, union, enum
  "struct",
  "union",
  "enum",
  // life time.
  "static",
  "extern",
};

const char *GetNameOfLabel(TokenLabel label) {
  return token_names[static_cast<int>(label)];
}

static TokenLabel GetLabelOfChar(char ch) {
  switch (ch) {
  case (';'): {
    return TokenLabel::TSEMICOLON;
  }
  case ('('): {
    return TokenLabel::TLEFTPARENT;
  }
  case (')'): {
    return TokenLabel::TRIGHTPARENT;
  }
  case ('['): {
    return TokenLabel::TLEFTSQ;
  }
  case (']'): {
    return TokenLabel::TRIGHTSQ;
  }
  case ('{'): {
    return TokenLabel::TLEFTBRACKET;
  }
  case ('}'): {
    return TokenLabel::TRIGHTBRACKET;
  }
  case (':'): {
    return TokenLabel::TCOLON;
  }
  case ('?'): {
    return TokenLabel::TQUESTION;
  }
  case ('#'): {
    return TokenLabel::TSHARP;
  }

  default: break;
  }

  static const char *printable = "~!%^&*-+=|,.<>?:/";
  for (const char *pt = printable; *pt; pt++) {
    if (ch == *pt) {
      return TokenLabel::TOPERATOR;
    }
  }

  return TokenLabel::TNULL;
}

static bool IsIdentifier(char ch) {
  // c identifier is made of alpha, digits, and _.
  return (ch >= 'a' && ch <='z') ||
         (ch >= 'A' && ch <= 'Z') ||
         (ch >= '0' && ch <= '9') || 
         (ch == '_');
}

static size_t FindNextChar(const std::string &str, size_t from, char ch) {
  size_t ret = from + 1;
  while (ret < str.size()) {
    if (str[ret] == ch) {
      break;
    }
    if (str[ret] == '\n') {
      lno++;
    }
    if (str[ret] == '\\') {
      // skip one char.
      ret ++;
      if (str[ret] == '\n') {
        lno ++;
      }
    }
    ret ++;
  }

  return ret > str.size() ? str.size() : ret;
}

static std::vector<Token> MergeEmptyTokens(std::vector<Token> &tokens);
static void ReLabelTokens(std::vector<Token> &tokens);

auto CLangTokenize(const std::string &fobj, bool IgnoreNull) -> std::vector<Token> {
  lno = 1; // reset line number
  size_t i = 0;
  const size_t len = fobj.size(); // length of file
  std::vector<Token> tokens = {};

  for (; i < len; ) {
    char ch = fobj[i];
    const auto oldno = lno;

    if (IsIdentifier(ch)) {
      size_t j;
      for (j = i + 1; j < len && IsIdentifier(fobj[j]); j++) {}
      tokens.push_back({fobj.substr(i, j - i), TokenLabel::TALPHA, lno});

      // advance
      i = j;
    } else {

      switch (ch) {

      // single quote
      case '\'': {
        size_t j = FindNextChar(fobj, i, ch);
        j++;
        tokens.push_back({fobj.substr(i, j - i), TokenLabel::TQUOTE, oldno});
        i = j;
        break;
      }

      // double quote
      case '\"': {
        size_t j = FindNextChar(fobj, i, ch);
        j++;
        tokens.push_back({fobj.substr(i, j - i), TokenLabel::TDOUBLEQUOTE, oldno});
        i = j;
        break;
      }

      // preprocessor commands.
      case '#': {
        size_t j = FindNextChar(fobj, i, '\n');
        tokens.push_back({fobj.substr(i, j - i), TokenLabel::TNULL, oldno});
        i = j;
        break;
      }

      // maybe comment
      case '/': {
        char next = (i + 1 >= len) ? '\0' : fobj[i + 1];
        if (next == '/') {
          size_t j = FindNextChar(fobj, i, '\n');
          tokens.push_back({fobj.substr(i, j - i), TokenLabel::TNULL, oldno});
          i = j;
        } else {
          if (next == '*') {
            // handle block comment
            size_t j = i + 2;
            while (j < len) {
              if (fobj[j] == '\n') { lno++; }
              // find close of block comment
              if (fobj[j] == '*' && fobj[j + 1] == '/') { break; }
              j++;
            }

            // scenario: *(j) /(j + 1) ?(j + 2)
            j+=2;
            tokens.push_back({fobj.substr(i, j - i), TokenLabel::TNULL, oldno});
            i = j;
          } else {
            // normal operator
            tokens.push_back({std::string(1, ch), TokenLabel::TOPERATOR, oldno});
            i++;
          }
        }
        break;
      }
 
      // new line
      case '\n': {
        tokens.push_back({std::string(1, ch), TokenLabel::TNULL, oldno});
        lno++; i++;
        break;
      }

      default: {
        // single-char operator or blank.
        tokens.push_back({std::string(1, ch), GetLabelOfChar(ch), oldno});
        i++;
        break;
      }

      }

    }
  }

  ReLabelTokens(tokens);
  if (IgnoreNull) {
    return RemoveNullTokens(tokens);
  }
  return MergeEmptyTokens(tokens);
}

static std::vector<Token> MergeEmptyTokens(std::vector<Token> &tokens)
{
  std::vector<Token> ret;

  size_t i = 0;
  const TokenLabel tnul = TokenLabel::TNULL;
  while (i < tokens.size()) {
    if (tokens[i].label != tnul) {
      ret.push_back(tokens[i]);
      i++;
    } else {
      size_t j =  i;
      uint32_t l = tokens[i].line;
      std::string buf;
      while (j < tokens.size()) {
        if (tokens[j].label != tnul) { break; }
        buf += tokens[j].buf;
        j++;
      }

      ret.push_back({buf, tnul, l});
      i = j;
    }
  }

  assert(ret.size() <= tokens.size());
  // free previous token
  tokens.clear();
  return ret;
}

auto RemoveNullTokens(std::vector<Token> &tokens) -> std::vector<Token> {
  std::vector<Token> ret;
  const TokenLabel tnul = TokenLabel::TNULL;

  for (auto &t : tokens) {
    if (t.label != tnul) { ret.push_back(t); }
  }

  tokens.clear();
  return ret;
}

static void ReLabelTokens(std::vector<Token> &tokens) {
  std::vector<Token> tmp;
  static const Token tnul = {"", TokenLabel::TNULL, 0};

  size_t i = 0;
  const size_t len = tokens.size();
  // in case of oob,
  tokens.push_back(tnul);
  assert(tokens.size() == len + 1);

  while (i < len) {
    const Token &t = tokens[i];
    switch (t.label) {
  
    case (TokenLabel::TALPHA): {
      // maybe one of the keyword
      bool matched = false;
      if (t.buf == "if") {
        tmp.push_back(Token(t.buf, TokenLabel::TIF, t.line));
        matched = true;
      }
      if (t.buf == "else") {
        tmp.push_back(Token(t.buf, TokenLabel::TELSE, t.line));
        matched = true;
      }
      if (t.buf == "while") {
        tmp.push_back(Token(t.buf, TokenLabel::TWHILE, t.line));
        matched = true;
      }
      if (t.buf == "return") {
        tmp.push_back(Token(t.buf, TokenLabel::TRETURN, t.line));
        matched = true;
      }
      if (t.buf == "for") {
        tmp.push_back(Token(t.buf, TokenLabel::TFOR, t.line));
        matched = true;
      }
      if (t.buf == "do") {
        tmp.push_back(Token(t.buf, TokenLabel::TDO, t.line));
        matched = true;
      }
      if (t.buf == "switch") {
        tmp.push_back(Token(t.buf, TokenLabel::TSWITCH, t.line));
        matched = true;
      }
      if (t.buf == "case") {
        tmp.push_back(Token(t.buf, TokenLabel::TCASE, t.line));
        matched = true;
      }
      if (t.buf == "default") {
        tmp.push_back(Token(t.buf, TokenLabel::TDEFAULT, t.line));
        matched = true;
      }
      if (t.buf == "break") {
        tmp.push_back(Token(t.buf, TokenLabel::TBREAK, t.line));
        matched = true;
      }
      if (t.buf == "continue") {
        tmp.push_back(Token(t.buf, TokenLabel::TCONTINUE, t.line));
        matched = true;
      }
      if (t.buf == "void") {
        tmp.push_back(Token(t.buf, TokenLabel::TVOID, t.line));
        matched = true;
      }
      if (t.buf == "long") {
        tmp.push_back(Token(t.buf, TokenLabel::TLONG, t.line));
        matched = true;
      }
      if (t.buf == "signed") {
        tmp.push_back(Token(t.buf, TokenLabel::TSIGNED, t.line));
        matched = true;
      }
      if (t.buf == "unsigned") {
        tmp.push_back(Token(t.buf, TokenLabel::TUNSIGNED, t.line));
        matched = true;
      }
      if (t.buf == "short") {
        tmp.push_back(Token(t.buf, TokenLabel::TSHORT, t.line));
        matched = true;
      }
      if (t.buf == "int") {
        tmp.push_back(Token(t.buf, TokenLabel::TINT, t.line));
        matched = true;
      }
      if (t.buf == "bool") {
        tmp.push_back(Token(t.buf, TokenLabel::TBOOL, t.line));
        matched = true;
      }
      if (t.buf == "char") {
        tmp.push_back(Token(t.buf, TokenLabel::TCHAR, t.line));
        matched = true;
      }
      if (t.buf == "struct") {
        tmp.push_back(Token(t.buf, TokenLabel::TSTRUCT, t.line));
        matched = true;
      }
      if (t.buf == "union") {
        tmp.push_back(Token(t.buf, TokenLabel::TUNION, t.line));
        matched = true;
      }
      if (t.buf == "enum") {
        tmp.push_back(Token(t.buf, TokenLabel::TENUM, t.line));
        matched = true;
      }
      if (t.buf == "static") {
        tmp.push_back(Token(t.buf, TokenLabel::TSTATIC, t.line));
        matched = true;
      }
      if (t.buf == "extern") {
        tmp.push_back(Token(t.buf, TokenLabel::TEXTERN, t.line));
        matched = true;
      }
      if (!matched) {
        // distinguish alpha and digit.
        char leading = t.buf[0];
        if (leading >= '0' && leading <= '9') {
          tmp.push_back(Token(t.buf, TokenLabel::TDIGIT, t.line));
        } else {
          tmp.push_back(t);
        }
      }

      i++;
      break;
    }

    case (TokenLabel::TOPERATOR): {
      const Token &next = tokens[i + 1];
      assert(t.buf.size() == 1);
      switch (t.buf[0]) {
      case ('/'): {
        if (next.buf == "=") {
          tmp.push_back(Token("/=", TokenLabel::TDIVBY, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TDIV, t.line));
          i++;
        }
        break;
      }
      case ('%'): {
        if (next.buf == "=") {
          tmp.push_back(Token("%=", TokenLabel::TREMBY, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TREM, t.line));
          i++;
        }
        break;
      }
      case ('*'): {
        if (next.buf == "=") {
          tmp.push_back(Token("*=", TokenLabel::TMULBY, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TMUL, t.line));
          i++;
        }
        break;
      }
      case ('^'): {
        if (next.buf == "=") {
          tmp.push_back(Token("^=", TokenLabel::TXORBY, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TXOR, t.line));
          i++;
        }
        break;
      }
      case ('.'): {
        tmp.push_back(Token(t.buf, TokenLabel::TDOT, t.line));
        i++;
        break;
      }
      case (','): {
        tmp.push_back(Token(t.buf, TokenLabel::TCOMMA, t.line));
        i++; break;
      }
      case ('~'): {
        tmp.push_back(Token(t.buf, TokenLabel::TFLIP, t.line));
        i++;
        break;
      }

      case ('+'): {
        // should handle +, += and ++
        if (next.buf == "+") {
          // is ++
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token("++", TokenLabel::TINCR, t.line));
          i += 2;
        } else if (next.buf == "=") {
          tmp.push_back(Token("+=", TokenLabel::TADDBY, t.line));
          i += 2;
        } else {
          // is +
          tmp.push_back(Token(t.buf, TokenLabel::TADD, t.line));
          i ++;
        }
        break;
      }

      case ('-'): {
        // should handle -, --, ->, -=
        if (next.buf == "-") {
          // is --
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token("--", TokenLabel::TDECR, t.line));
          i += 2;
        } else if (next.buf == ">") {
          // is ->
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token("->", TokenLabel::TARROW, t.line));
          i += 2;
        } else if (next.buf == "=") {
          tmp.push_back(Token("-=", TokenLabel::TSUBBY, t.line));
          i += 2;
        } else {
          // is -
          tmp.push_back(Token(t.buf, TokenLabel::TSUB, t.line));
          i++;
        }

        break;
      }

      case ('='): {
        // should handle =, ==
        if (next.buf == "=") {
          // is ==
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token("==", TokenLabel::TEQ, t.line));
          i += 2;
        } else {
          // is =
          tmp.push_back(Token(t.buf, TokenLabel::TASSIGN, t.line));
          i++;
        }

        break;
      }
      case ('!'): {
        // should handle !, !=
        if (next.buf == "=") {
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token("!=", TokenLabel::TNE, t.line));
          i += 2;
        } else {
          tmp.push_back(Token("!", TokenLabel::TNOT, t.line));
          i++;
        }
        break;
      }

      case ('>'): {
        // should handle >, >=
        if (next.buf == "=") {
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token(">=", TokenLabel::TGEQ, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TGE, t.line));
          i++;
        }

        break;
      }

      case ('<'): {
        if (next.buf == "=") {
          assert(next.label == TokenLabel::TOPERATOR);
          tmp.push_back(Token("<=", TokenLabel::TLEQ, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TLE, t.line));
          i++;
        }

        break;
      }

      case ('&'): {
        if (next.buf == "&") {
          // is &&
          tmp.push_back(Token("&&", TokenLabel::TAND, t.line));
          i += 2;
        } else if (next.buf == "=") {
          tmp.push_back(Token("&=", TokenLabel::TANDBY, t.line));
          i += 2;
        } else {
          // is &
          tmp.push_back(Token(t.buf, TokenLabel::TADRP, t.line));
          i ++;
        }
        break;
      }
      case ('|'): {
        if (next.buf == "|") {
          tmp.push_back(Token("||", TokenLabel::TOR, t.line));
          i += 2;
        } else if (next.buf == "=") {
          tmp.push_back(Token("|=", TokenLabel::TORBY, t.line));
          i += 2;
        } else {
          tmp.push_back(Token(t.buf, TokenLabel::TPIPE, t.line));
          i ++;
        }
        break;
      }

      default: {
        // should not happen.
        fprintf(stderr, "Unknown operator at line %u: %s\n", t.line, EncodeString(t.buf).c_str());
        fprintf(stderr, "Further execution is not possible.\n");
        assert(0);
      }
      }
      break;
    }

    default:  {
      tmp.push_back(t);
      i++;
      break;
    }
    }
  }

  tokens.clear();
  tokens = tmp;
}

} // namespace Lex

namespace Parser {

static const char *block_type_names[] = {
  "common",
  "function",
  "if",
  "else",
  "while",
  "var",
  "ret",
  "if-else",
  "for",
  "do",
  "switch",
  "case",
  "break",
  "continue",
  "struct",
  "union",
  "enum",
};

auto BlockTypeToString(BlockType bt) -> std::string {
  return block_type_names[static_cast<int>(bt)];
}

static size_t ident = 0;
static inline auto PrintIdent(std::ostream &os) -> std::ostream & {
  // avoid underflow
  assert(ident < max_recursion);
  os << std::string(ident * 2, ' ');
  return os;
}

auto Instruction::GetFuncCalls() const -> std::vector<std::string> {
  size_t len = this->tokens.size();
  if (len == 0) {
    return {};
  }

  len --;

  std::vector<std::string> ret;
  for (size_t i = 0; i < len; i++) {
    if (tokens[i].label == Lex::TokenLabel::TALPHA && 
        tokens[i + 1].label == Lex::TokenLabel::TLEFTPARENT) {
      ret.push_back(tokens[i].buf);
    }
  }
  return ret;
}

// non-recursive
static void AddLabelForBlock(BasicBlock *root) {
  // there're two types of BasicBlock:
  // one is simply a wrapper of an instruction;
  // the other is the collection of basic blocks.
  // assert(root->GetInstruction().tokens.empty() || root->GetNumChildren() == 0);
  // find the first token that is not null.

  if (root->GetInstruction().tokens.empty()) {
    // assert(root->GetType() == BlockType::BCOMMON);
    return;
  }

  const auto &Tokens = root->GetInstruction().tokens;
  for (const auto &token : Tokens) {
    if (token.label == Lex::TokenLabel::TNULL) {
      continue;
    }

    switch (token.label) {
    case (Lex::TokenLabel::TCASE):
    case (Lex::TokenLabel::TDEFAULT): {
      root->SetType(BlockType::BCASE);
      return;
    }
    case (Lex::TokenLabel::TSWITCH): {
      root->SetType(BlockType::BSWITCH);
      break;
    }
    case (Lex::TokenLabel::TVOID):
    case (Lex::TokenLabel::TINT):
    case (Lex::TokenLabel::TCHAR):
    case (Lex::TokenLabel::TBOOL): {
      // function declaration or definition
      size_t ln = Tokens.size();
      if (Tokens.size() > 3 && Tokens[ln - 1].label != Lex::TokenLabel::TSEMICOLON) {
        // is a function declaration, for example:
        // int func (
        root->SetType(BlockType::BFUNCTION);
      } else {
        root->SetType(BlockType::BVARDECLARE);
      }
      return;
    }

    // control flows
    case (Lex::TokenLabel::TIF): {
      root->SetType(BlockType::BIF); 
      return;
    }
    case (Lex::TokenLabel::TWHILE): {
      root->SetType(BlockType::BWHILE);
      return;
    }
    case (Lex::TokenLabel::TDO): {
      root->SetType(BlockType::BDO);
      break;
    }
    case (Lex::TokenLabel::TFOR): {
      root->SetType(BlockType::BFOR);
      break;
    }
    case (Lex::TokenLabel::TBREAK): {
      root->SetType(BlockType::BBREAK);
      break;
    }
    case (Lex::TokenLabel::TCONTINUE): {
      root->SetType(BlockType::BCONTINUE);
      break;
    }
    case (Lex::TokenLabel::TELSE): {
      root->SetType(BlockType::BELSE);
      return;
    }
    case (Lex::TokenLabel::TRETURN): {
      root->SetType(BlockType::BRET);
      return;
    }

    case (Lex::TokenLabel::TSTRUCT): {
      root->SetType(BlockType::BSTRUCT);
      break;
    }
    case (Lex::TokenLabel::TUNION): {
      root->SetType(BlockType::BUNION);
      break;
    }
    case (Lex::TokenLabel::TENUM): {
      root->SetType(BlockType::BENUM);
      break;
    }

    default: {
      break;
    }
    }
  }
}

// recursive, should be called only once for each tree
static void AddLabelForBlocks(BasicBlock *root) {
  assert(root != nullptr);
  // assert(root->GetType() == BlockType::BCOMMON);

  AddLabelForBlock(root);
  size_t nchildren = root->GetNumChildren();
  for (size_t i = 0; i < nchildren; i++) {
    AddLabelForBlocks(root->GetChild(i));
  }
}

void BasicBlock::ReshapeBlock(BasicBlock *root) {
  std::vector<BasicBlock *> new_children;
  size_t num_children = root->children.size();
  size_t i = 0;
  while (i < num_children) {
    BasicBlock *child = root->children[i];
    switch (child->btype) {
    case (BlockType::BDO): {
      // do { do_something(); }
      i += 1;
      if (i < num_children) {
        child->children.push_back(root->children[i]);
      }

      // while (some_cond);
      i += 1;
      if (i < num_children) {
        child->children.push_back(root->children[i]);
      }
      break;
    }

    case (BlockType::BFUNCTION):
    case (BlockType::BIF):
    case (BlockType::BELSE):
    case (BlockType::BFOR):
    case (BlockType::BWHILE): {
      // i += 1;
      // reparent the next basic block to this one.
      assert(child->children.size() == 0);
      const auto &insn = child->instruction;
      size_t l = insn.tokens.size();
      if (i + 1 < num_children && l > 0 && 
          insn.tokens[l - 1].label != Lex::TokenLabel::TSEMICOLON) {
        i += 1;
        child->children.push_back(root->children[i]);
      }
      break;
    }

    default: {
      break;
    }
    }

    new_children.push_back(child);
    i ++;
  }

  root->children.clear();
  root->children = new_children;
}

void BasicBlock::ReshapeBlockTree(BasicBlock *root) {
  for (auto *child : root->children) {
    ReshapeBlockTree(child);
  }
  ReshapeBlock(root);
}
void BasicBlock::MergeIfElseBlock(BasicBlock *root) {
  std::vector<BasicBlock *> new_children;
  size_t num_children = root->children.size();
  size_t i = 0;

  while  (i < num_children) {
    auto *child = root->children[i];
    if (child->GetType() == BlockType::BIF) {
      assert(child->GetNumChildren() == 1);
      if (i + 1 < num_children && root->children[i + 1]->GetType() == 
          BlockType::BELSE) {
        auto *next = root->children[i + 1];
        assert(next->GetNumChildren() == 1);
        auto if_else_block = new BasicBlock();  
        if_else_block->SetType(BlockType::BIFELSE);
        if_else_block->instruction = child->instruction;
        if_else_block->children = {child->children[0], next->children[0]};
        new_children.push_back(if_else_block);

        // will not be used later.
        child->children.clear(); delete child;
        next->children.clear(); delete next;
        i += 2;
      } else {
        new_children.push_back(child);
        i++;
      }
    } else {
      new_children.push_back(child);
      i++;
    }
  }

  for (auto *child : new_children) {
    if (child->GetType() == BlockType::BELSE) {
      fprintf(stderr, "Else block is not followed by if block\n");
      assert(false);
    }
  }

  root->children.clear();
  root->children = new_children;
}
void BasicBlock::MergeIfElseBlockTree(BasicBlock *root) {
  for (auto child : root->children) {
    MergeIfElseBlockTree(child);
  }
  MergeIfElseBlock(root);
}

auto BasicBlock::GetLineRange() const -> std::pair<size_t, size_t> {
  std::pair<size_t, size_t> ret = {-1, 0};
  if (!this->instruction.tokens.empty()) {
    ret = this->instruction.GetLineRange();
  }

  for (auto *child : this->children) {
    auto rg = child->GetLineRange();
    if (rg.first < ret.first) {
      ret.first = rg.first;
    }
    if (rg.second > ret.second) {
      ret.second = rg.second;
    }
  }

  return ret;
}

static BasicBlock *CLangParseRecur(const std::vector<Lex::Token> &tokens, 
                                   size_t *index) {
  BasicBlock *top = new BasicBlock();
  assert (index != nullptr);
  assert (top != nullptr);
  Instruction instr;

  while (*index < tokens.size()) {
    switch (tokens[*index].label) {
    case Lex::TokenLabel::TLEFTBRACKET: {
      // end of an instruction.
      if (!instr.tokens.empty()) {
        top->AddChild(new BasicBlock(instr));
        instr.tokens.clear();
      }
      *index += 1;
      BasicBlock *child = CLangParseRecur(tokens, index);
      child->HasBracket();
      top->AddChild(child);
      break;
    }
    case Lex::TokenLabel::TRIGHTBRACKET: {
      // finished this block
      // return to the caller.
      if (!instr.tokens.empty()) {
        top->AddChild(new BasicBlock(instr));
        instr.tokens.clear();
      }
      *index += 1;
      return top;
    }

    case Lex::TokenLabel::TSEMICOLON: {
      // finished this instruction
      // add it to the block
      instr.tokens.push_back(tokens[*index]);
      if (!instr.tokens.empty()) {
        top->AddChild(new BasicBlock(instr));
        instr.tokens.clear();
      }
      *index += 1;
      break;
    }

    case Lex::TokenLabel::TNULL: {
      // discard.
      *index += 1;
      break;
    }

    default: {
      instr.tokens.push_back(tokens[*index]);
      *index += 1;
      break;
    }
    }
  }

  AddLabelForBlocks(top);
  BasicBlock::ReshapeBlockTree(top);
  return top;
}

auto Instruction::Print(std::ostream &os) const -> std::ostream & {
  PrintIdent(os);
  for (const auto &token : tokens) {
    os << token.buf;
    os << TO_STD_STRING(" ");
  }
  os << TO_STD_STRING("\n");
  return os;
}

auto BasicBlock::Print(std::ostream &os) const -> std::ostream & {
  // after reshaping, this assertion may not be true.
  // assert(this->instruction.tokens.empty() || 
  //        this->children.empty());

  PrintIdent(os);
  if (!this->instruction.tokens.empty()) {
    this->instruction.Print(os);
    // return os;
  }

  if (!this->children.empty()) {

    os << TO_STD_STRING("\n");
    if (this->has_bracket_) {
      PrintIdent(os);
      os << TO_STD_STRING("{\n");
    }
  
    ++ident;
    for (const auto *child : this->children) {
      child->Print(os);
    }
  
    --ident;
    if (this->has_bracket_) {
      PrintIdent(os);
      os << TO_STD_STRING("}\n");
    }
  } else {
    if (this->has_bracket_) {
      ++ident;
      os << TO_STD_STRING("\n");
      PrintIdent(os);
      os << TO_STD_STRING("{ }\n");
      --ident;
    }
  }
  return os;
}

auto CLangParser(const std::vector<Lex::Token> &tokens) -> BasicBlock * {
  size_t idx = 0;
  // recursively parse the tokens
  return CLangParseRecur(tokens, &idx);
}

} // namespace Parser

namespace Generator {

static const char *X86Regs8Bit[] = {
  "al",
  "bl",
  "cl",
  "dl",
  "sil",
  "dil",
  "bpl",
  "spl",
  "r8b",
  "r9b",
  "r10b",
  "r11b",
  "r12b",
  "r13b",
  "r14b",
  "r15b",
};
static const char *X86Regs32Bit[] = {
  "eax",
  "ebx",
  "ecx",
  "edx",
  "esi",
  "edi",
  "ebp",
  "esp",
  "r8d",
  "r9d",
  "r10d",
  "r11d",
  "r12d",
  "r13d",
  "r14d",
  "r15d",
};

static const char *X86Regs64Bit[] = {
  "rax",
  "rbx",
  "rcx",
  "rdx",
  "rsi", // 4
  "rdi",
  "rbp",
  "rsp",
  "r8",
  "r9", // 9
  "r10",
  "r11",
  "r12",
  "r13",
  "r14", // 14
  "r15",
};

auto StackFrame::AllocDoubleWord() -> size_t {
  if (!this->free_list_of_dword.empty()) {
    size_t ret = this->free_list_of_dword.front();
    this->free_list_of_dword.pop();
    assert(ret >= 8 && ret % 8 == 0);
    return ret;
  }

  // stack pointer must align to 16 bytes.
  this->alloc_size += 16;
  this->free_list_of_dword.push(this->alloc_size - 8);
  return (this->alloc_size);
}

auto StackFrame::AllocWord() -> size_t {
  if (!this->free_list_of_word.empty()) {
    size_t ret = this->free_list_of_word.front();
    this->free_list_of_word.pop();
    assert(ret >= 4 && ret % 4 == 0);
    return ret;
  }

  size_t dword = this->AllocDoubleWord();
  this->free_list_of_word.push(dword - 4);
  return dword;
}

auto StackFrame::AllocByte() -> size_t {
  if (!this->free_list_of_byte.empty()) {
    size_t ret = this->free_list_of_byte.front();
    this->free_list_of_byte.pop();
    assert(ret >= 1);
    return ret;
  }

  size_t word = this->AllocWord();
  this->free_list_of_byte.push(word - 1);
  this->free_list_of_byte.push(word - 2);
  this->free_list_of_byte.push(word - 3);
  return word;
}

auto StackFrame::Alloc(size_t size) -> size_t {
  assert(size != 0);
  if (size == 1) {
    return this->AllocByte();
  }
  if (size <= 4) {
    return this->AllocWord();
  }
  if (size <= 8) {
    return this->AllocDoubleWord();
  }

  fprintf(stderr, "Warning: allocating large object of %ld bytes\n", size);
  size = (size + 15) & (~15);
  // FIXME: if the size of the object is not a multiple of 16 bytes,
  // then there are fragmentation.
  this->alloc_size += size;
  return this->alloc_size;
}

auto SymbolTable::Lookup(const std::string &name) -> SymbolType * {
  for (auto it = this->table_stack.rbegin(); it != this->table_stack.rend(); 
       ++it) {
    auto table = *it;
    auto it2 = table.find(name);
    if (it2 != table.end()) {
      return new SymbolType(it2->second);
    }
  }

  return nullptr;
}

auto X86Generator::GenerateCode(Parser::BasicBlock *root) -> std::string {
  std::ostringstream os;
  assert(root != nullptr);
  assert(root->GetType() == Parser::BlockType::BCOMMON);

  // initial stack size
  assert(this->symtab.GetStackSize() == 0);

  // FIXME: .file source_file.c
  this->GenerateCodeForBlock(os, root);

  // dump all c string consts into asm code
  this->DumpCString(os);
  return os.str();
}

auto X86Generator::GenerateCodeForBlock(std::ostringstream &os, Parser::BasicBlock *block) 
  -> std::ostringstream & {
  assert(block != nullptr); 

  auto instr = block->GetInstruction();
  switch (block->GetType()) {
  case (Parser::BlockType::BCOMMON): {
    if (instr.tokens.size() > 0) {
      // eg: ret = ret + 1;
      this->GenerateCodeForInstruction(os, instr);
    } else {
      // is root block
      this->symtab.Enter(os);
      auto num_children = block->GetNumChildren();
      for (size_t i = 0; i < num_children; i++) {
        this->GenerateCodeForBlock(os, block->GetChild(i));
        // os << TO_STD_STRING("\n");
      }
      // leave this block, let the symbol table recover
      // stack pointer.
      this->symtab.Leave(os);
    }
    break;
  }
  case (Parser::BlockType::BFUNCTION): {
    assert(!instr.tokens.empty());
    assert(block->GetNumChildren() == 1);
    // FIXME: not all functions should be labeled 'global'.

    // example asm code:
    // .text
    // .globl main
    os << TO_STD_STRING("\n\t.text\n");
    os << TO_STD_STRING("\t.globl ");
    os << instr.tokens[1].buf;
    os << TO_STD_STRING("\n");

    // .type main, @function
    os << TO_STD_STRING("\t.type ");
    os << instr.tokens[1].buf;
    os << TO_STD_STRING(", @function\n");

    // main:
    os << instr.tokens[1].buf;
    os << TO_STD_STRING(":\n");

    // endbr64
    os << TO_STD_STRING("\tendbr64\n");

    // this->GenerateCodeForInstruction(os, block->GetInstruction());
    this->symtab.Enter(os);
    this->StoreArgsIntoMem(os, block->GetInstruction());
    this->GenerateCodeForBlock(os, block->GetChild(0));
    this->symtab.Leave(os);
    // force return
    os << TO_STD_STRING("\tret\n");
    break;
  }
  case (Parser::BlockType::BWHILE): {
    // example source:
    // while ( some_var ) {
    //   do_something();
    // }
    // example asm:
    // .L2:
    //   cmpl $0, %rax
    //   je .L3
    //   <code in the while loop>
    //   jmp .L2
    // .L3:
    assert(!instr.tokens.empty());
    assert(block->GetNumChildren() == 1);
    size_t enter_label = this->branch_count++;
    size_t leave_label = this->branch_count++;
    os << ".L" << enter_label << ":\n";
    assert(instr.tokens.size() == 4);
    this->LoadValueIntoReg(os, instr.tokens[2], X86Registers::AX);
    os << "\tcmp $0, %rax\n";
    os << "\tje .L" << leave_label << "\n";
    this->GenerateCodeForBlock(os, block->GetChild(0));
    os << "\tjmp .L" << enter_label << "\n";
    os << ".L" << leave_label << ":" << "\n";
    break;
  }
  case (Parser::BlockType::BIF): {
    assert(!instr.tokens.empty());
    assert(block->GetNumChildren() == 1);
    // example source:
    // if (some_var) 
    //   do_something();
    // example assembly:
    // cmp $0, %rax
    // je .L1
    // do_something()
    // .L1:
    size_t leave_label = this->branch_count++;
    this->LoadValueIntoReg(os, instr.tokens[2], X86Registers::AX);
    os << "\tcmp $0, %rax\n";
    os << "\tje .L" << leave_label << "\n";
    this->GenerateCodeForBlock(os, block->GetChild(0));
    os << ".L" << leave_label << ":" << "\n";
    break;
  }
  case (Parser::BlockType::BIFELSE): {
    // example source:
    // if (some_var) 
    //   do_something();
    // else
    //   do_something_else();
    // expected assembly:
    // cmp $0, %rax
    // je .Lelse_label
    //  call do_something
    //  j .Lend_label
    // .Lelse_label:
    //  call do_something_else
    // .Lend_label:
    assert(!instr.tokens.empty());
    assert(block->GetNumChildren() == 2);
    size_t else_label = this->branch_count++;
    size_t end_label = this->branch_count++;
    this->LoadValueIntoReg(os, instr.tokens[2], X86Registers::AX);
    os << "\tcmp $0, %rax\n";
    os << "\tje .L" << else_label << "\n";
    this->GenerateCodeForBlock(os, block->GetChild(0));
    os << "\tjmp .L" << end_label << "\n";
    os << ".L" << else_label << ":\n";
    this->GenerateCodeForBlock(os, block->GetChild(1));
    os << ".L" << end_label << ":\n";
    break;
  }
  case (Parser::BlockType::BVARDECLARE): {
    // example: int var; char **argv;
    // int arr[10];
    assert(block->GetNumChildren() == 0);
    SymbolType symtype;
    symtype.is_global = this->symtab.GetStackDepth() <= 1;

    switch(instr.tokens[0].label) {
    case (Lex::TokenLabel::TBOOL): {
      symtype.base_type = SymbolType::BaseType::TBOOL;
      break;
    }
    case (Lex::TokenLabel::TCHAR): {
      symtype.base_type = SymbolType::BaseType::TCHAR;
      break;
    }
    case (Lex::TokenLabel::TINT): {
      symtype.base_type = SymbolType::BaseType::TINT;
      break;
    }
    case (Lex::TokenLabel::TVOID): {
      symtype.base_type = SymbolType::BaseType::TVOID;
      break;
    }
    default: {
      fprintf(stderr, "Error: Invalid type\n");
      assert(false);
    }
    }

    // compute pointer level
    symtype.pointer_level = 0;
    size_t i;
    for (i = 1; instr.tokens[i].label == Lex::TokenLabel::TMUL; i++) {
      symtype.pointer_level++;
    }
    if (symtype.pointer_level == 0 && symtype.base_type == SymbolType::BaseType::TVOID) {
      fprintf(stderr, "cannot create scalar of void type.\n");
      assert(false);
    }
    
    // name of the var.
    const std::string &name = instr.tokens[i].buf;

    // check whether this var is an array.
    if (i + 1 < instr.tokens.size()) {
      assert(instr.tokens[i + 1].label == Lex::TokenLabel::TLEFTSQ);
      assert(instr.tokens[i + 3].label == Lex::TokenLabel::TRIGHTSQ);
      auto arr_size = Atoi(instr.tokens[i + 2].buf);
      assert(arr_size > 0);
      symtype.is_array = true;
      symtype.array_size = arr_size;
    }

    auto stack_frame = this->symtab.GetCurrentStackFrame();
    // the stack frame may allocate memory for the variable.
    size_t old_size = stack_frame->alloc_size;
    if (!symtype.is_global) {
      // should allocate on stack.
      // global vars are allocated in .bss
      symtype.addr = stack_frame->Alloc(symtype.MemorySize());
      if (stack_frame->alloc_size > old_size) {
        os << "\taddq $-" << stack_frame->alloc_size - old_size << ", %rsp\n";
      }
      symtype.stack_frame = stack_frame;
    } else {
      size_t mem_size = symtype.MemorySize();
      // example assembly:
      // .bss
      // .align 16
      // .type var_name, @object
      // .size var_name, mem_size
      // .globl var_name
      // var_name:
      // .zero mem_size
      os << "\n\t.bss\n";
      os << "\t.align 16\n";
      os << "\t.type " << name << ", @object\n";
      os << "\t.size " << name << ", " << mem_size << "\n";
      os << "\t.globl " << name << "\n";
      os << name << ":\n\t.zero " << mem_size << "\n";
      symtype.stack_frame = nullptr;
    }
    this->symtab.AddSymbol(name, symtype);
    break;
  }
  case (Parser::BlockType::BRET): {
    // return the value of a single variable or a const number.
    // example: return var; return 0; return;
    assert(instr.tokens.size() <= 2);
    assert(instr.tokens[0].label == Lex::TokenLabel::TRETURN);

    if (instr.tokens.size() == 2) {
      if (isdigit(instr.tokens[1].buf[0])) {
        os << "\tmovq $" << instr.tokens[1].buf << ", %rax\n";
      } else {
        this->LoadVarIntoReg(os, instr.tokens[1].buf, X86Registers::AX);
      }
    } else {
      // for functions like void func();
      // there's no return value.
      // assert(instr.tokens.size() == 1);
    }

    // recover stack pointer
    os << "\taddq $" << this->symtab.GetStackSize() << ", %rsp\n" ;
    os << "\tret\n";
    break;
  }

  case (Parser::BlockType::BELSE): {
    // this block should be cleared by BasicBlock::MergeIfElseBlockTree
    fprintf(stderr, "This is a bug. Please report it.\n");
    assert(false);
  }
  default: {
    fprintf(stderr, "Unknown block type\n");
    assert(false);
  }
  }

  return os;
}

auto X86Generator::LoadVarIntoReg(std::ostringstream &os, const std::string 
  &var_name, X86Registers reg) -> std::ostringstream & {
  assert(isalpha(var_name[0]) || var_name[0] == '_');

  auto *symtype = this->symtab.Lookup(var_name);
  if (symtype == nullptr) {
    fprintf(stderr, "Unknown variable %s\n", var_name.c_str());
    assert(false);
  }
  if (symtype->is_array) {
    fprintf(stderr, "Cannot load array into register\n");
    assert(false);
  }
  auto sp = this->symtab.GetStackSize();
  assert(sp >= symtype->GetAddr());
  size_t offset = sp - symtype->GetAddr();

  bool is_global = symtype->is_global;
  if (symtype->MemorySize() == 1) {
    // movzbl: byte to int, zero extend
    const char *reg_name = X86Regs8Bit[static_cast<int>(reg)];
    if (is_global) {
      os << "\tmovb " << var_name << "(%rip), %" << reg_name << "\n"; 
    } else {
      os << "\tmovb " << offset << "(%rsp), %" << reg_name << "\n";
    }
    os << "\tand $0xff, %rax\n";
  } else {
    if (symtype->MemorySize() == 4) {
      const char *reg_name = X86Regs32Bit[static_cast<int>(reg)];
      if (is_global) {
        os << "\tmovl " << var_name << "(%rip), %" << reg_name << "\n";
      } else {
        os << "\tmovl " << offset << "(%rsp), %" << reg_name << "\n";
      }
    } else {
      const char *reg_name = X86Regs64Bit[static_cast<int>(reg)];
      if (is_global) {
        os << "\tmovq " << var_name << "(%rip), %" << reg_name << "\n";
      } else {
        os << "\tmovq " << offset << "(%rsp), %" << reg_name << "\n";
      }
    }
  }

  delete symtype;
  return os;
}

auto X86Generator::StoreVarFromReg(std::ostringstream &os, const std::string 
  &var_name, X86Registers reg) -> std::ostringstream & {
  assert(isalpha(var_name[0]) || var_name[0] == '_');

  auto *symtype = this->symtab.Lookup(var_name);
  if (symtype == nullptr) {
    fprintf(stderr, "Unknown variable %s\n", var_name.c_str());
    assert(false);
  }
  if (symtype->is_array) {
    fprintf(stderr, "Cannot store array from register\n");
    assert(false);
  }
  auto sp = this->symtab.GetStackSize();
  assert(sp >= symtype->GetAddr());
  size_t offset = sp - symtype->GetAddr();

  bool is_global = symtype->is_global;
  if (symtype->MemorySize() == 1) {
    const char *reg_name = X86Regs8Bit[static_cast<int>(reg)];
    if (is_global) {
      os << "\tmovb %" << reg_name << ", " << var_name << "(%rip)\n";
    }
    else {
      os << "\tmovb %" << reg_name << ", " << offset << "(%rsp)\n";
    }
  } else {
    if (symtype->MemorySize() == 4) {
      const char *reg_name = X86Regs32Bit[static_cast<int>(reg)];
      if (is_global) {
        os << "\tmovl %" << reg_name << ", " << var_name << "(%rip)\n";
      } else {
        os << "\tmovl %" << reg_name << ", " << offset << "(%rsp)\n";
      }
    } else {
      const char *reg_name = X86Regs64Bit[static_cast<int>(reg)];
      if (is_global) {
        os << "\tmovq %" << reg_name << ", " << var_name << "(%rip)\n";
      } else {
        os << "\tmovq %" << reg_name << ", " << offset << "(%rsp)\n";
      }
    }
  }

  delete symtype;
  return os;
}

auto X86Generator::GenerateCodeForInstruction(std::ostringstream &os, const 
  Parser::Instruction &instr) -> std::ostringstream & {
  // example:
  // a = b;
  // a = 2;
  // a = b + 2;
  // a ++;
  // a = do_something_and_return();
  // do_something(a, b, c);

  // added after 
  // commit 490fef22ce982d94669d16b93bd7fa2028a0f276
  // a = "c style string";

  static const size_t max_args = 6;
  static const X86Registers function_args[] = {
    X86Registers::DI, // arg1: rdi,
    X86Registers::SI, // arg2: rsi
    X86Registers::DX, // arg3: rdx
    X86Registers::CX, // arg4: rcx
    X86Registers::R8, // arg5: r8
    X86Registers::R9, // arg6: r9
  };

  // check whether this is a function call.
  if ((instr.GetTypeOfToken(1) == Lex::TokenLabel::TLEFTPARENT)) {
    assert(instr.tokens.size() >= 3);
    // probably do_something(a, b, c);
    // assembly code:
    // call do_something

    // prepare arguments
    size_t nargs = 0;
    for (size_t i = 2; i < instr.tokens.size(); ) {
      if (instr.GetTypeOfToken(i) == Lex::TokenLabel::TRIGHTPARENT) {
        break;
      }

      // if the assertions fail, there's syntax error.
      assert(instr.GetTypeOfToken(i) == Lex::TokenLabel::TALPHA);
      assert(instr.GetTypeOfToken(i + 1) == Lex::TokenLabel::TCOMMA
        || instr.GetTypeOfToken(i + 1) == Lex::TokenLabel::TRIGHTPARENT);

      assert(nargs < max_args);
      this->LoadValueIntoReg(os, instr.tokens[i], function_args[nargs]);
      nargs++;
      i += 2;
    }

    os << "\tcall " << instr.tokens[0].buf << "\n";
    return os;
  } 

  if (instr.GetTypeOfToken(1) == Lex::TokenLabel::TASSIGN && 
      instr.GetTypeOfToken(3) == Lex::TokenLabel::TLEFTPARENT) {
    // probably ret = do_something_and_return ( );
    assert(instr.tokens.size() >= 5);

    // prepare arguments
    size_t nargs = 0;
    for (size_t i = 4; i < instr.tokens.size(); ) {
      if (instr.GetTypeOfToken(i) == Lex::TokenLabel::TRIGHTPARENT) {
        break;
      }

      // if the assertions fail, there's syntax error.
      assert(instr.GetTypeOfToken(i) == Lex::TokenLabel::TALPHA);
      assert(instr.GetTypeOfToken(i + 1) == Lex::TokenLabel::TCOMMA
        || instr.GetTypeOfToken(i + 1) == Lex::TokenLabel::TRIGHTPARENT);

      assert(nargs < max_args);
      this->LoadValueIntoReg(os, instr.tokens[i], function_args[nargs]);
      nargs++;
      i += 2;
    }

    os << "\tcall " << instr.tokens[2].buf << "\n";
    // store the return value to memory
    this->StoreVarFromReg(os, instr.tokens[0].buf, X86Registers::AX);
    return os;
  }

  if (instr.GetTypeOfToken(0) == Lex::TokenLabel::TMUL) {
    // probably *pt = some_val;
    assert(instr.tokens.size() == 4);
    this->LoadValueIntoReg(os, instr.tokens[3], X86Registers::R10);
    this->LoadValueIntoReg(os, instr.tokens[1], X86Registers::AX);
    const char *r10 = nullptr;
    const char *mov = nullptr;

    auto *symtype_ptr = this->symtab.Lookup(instr.tokens[1].buf);
    assert(symtype_ptr != nullptr);
    size_t memsz = 0;
    if (symtype_ptr->pointer_level > 1) {
      memsz = sizeof(void *);
    } else {
      switch (symtype_ptr->base_type) {
      case (SymbolType::BaseType::TBOOL):
      case (SymbolType::BaseType::TCHAR): {
        memsz = sizeof(char);
        break;
      }
      case (SymbolType::BaseType::TINT): {
        memsz = sizeof(int);
        break;
      }
      default: {
        fprintf(stderr, "Unsupported type\n");
        assert(0);
      }
      }
    }
    delete symtype_ptr;

    switch (memsz) {
    case (1): {
      mov = "\tmovb %";
      r10 = X86Regs8Bit[static_cast<int>(X86Registers::R10)];
      break;
    }
    case (4): {
      mov = "\tmovl %";
      r10 = X86Regs32Bit[static_cast<int>(X86Registers::R10)];
      break;
    }
    case (8): {
      mov = "\tmovq %";
      r10 = X86Regs64Bit[static_cast<int>(X86Registers::R10)];
      break;
    }
    default:
      break;
    }
    assert(r10 != nullptr);

    os << mov << r10 << ", " << "(%rax)\n";
    return os;
  }

  if (instr.tokens.size() == 2) {
    // FIXME: will simply load the value 
    // into register %rax.
    size_t incr = 1;
    this->LoadVarIntoReg(os, instr.tokens[0].buf, X86Registers::AX);
    auto *symtype = this->symtab.Lookup(instr.tokens[0].buf);

    if (symtype->is_array) {
      fprintf(stderr, "Array assignment not supported");
      assert(false);
    }

    if (symtype->pointer_level > 1) {
      incr = sizeof(void *);
    } else {
      if (symtype->pointer_level == 1) {
        switch (symtype->base_type) {
        case (SymbolType::BaseType::TCHAR):
        case (SymbolType::BaseType::TBOOL): {
          break;
        }

        case (SymbolType::BaseType::TINT): {
          incr = sizeof(int);
          break;
        }
        default: {
          break;
        }
        }
      }
    }

    delete symtype;

    switch (instr.tokens[1].label) {
    case (Lex::TokenLabel::TINCR): {
      os << "\taddq $" << incr << ", %rax\n";
      break;
    }
    case (Lex::TokenLabel::TDECR): {
      os << "\taddq $-" << incr << ", %rax\n";
      break;
    }
    default: {
      fprintf(stderr, "unknown unary operator %s\n", 
              Lex::GetNameOfLabel(instr.tokens[1].label));
      assert(false);
    }
    }

    this->StoreVarFromReg(os, instr.tokens[0].buf, X86Registers::AX);
    return os;
  }

  if (instr.tokens.size() == 3) {
    // must be assignment
    // example: a = foo; a = 2;
    assert(instr.tokens[1].label == Lex::TokenLabel::TASSIGN);
    this->LoadValueIntoReg(os, instr.tokens[2], X86Registers::AX);
    // store the value in the variable
    this->StoreVarFromReg(os, instr.tokens[0].buf, X86Registers::AX);
    return os;
  }

  if (instr.tokens.size() == 4) {
    // get address, or deref a pointer
    assert(instr.tokens[1].label == Lex::TokenLabel::TASSIGN);

    switch (instr.tokens[2].label) {
    case (Lex::TokenLabel::TADD): {
      // no operation required
      this->LoadValueIntoReg(os, instr.tokens[3], X86Registers::AX);
      break;
    }
    case (Lex::TokenLabel::TMUL): {
      // deref a pointer 
      this->LoadValueIntoReg(os, instr.tokens[3], X86Registers::AX);
      os << "\tmov (%rax), %rax\n";
      break;
    }
    case (Lex::TokenLabel::TADRP): {
      // get the address of the operand
      // example: pt = &a;
      auto *symtype = this->symtab.Lookup(instr.tokens[3].buf);
      if (symtype->is_global) {
        // asm code: leaq a(%rip), %rax
        os << "\tleaq " << instr.tokens[3].buf << "(%rip), %rax\n";
      } else {
        // the value is on stack.
        auto sp = this->symtab.GetStackSize();
        assert(sp >= symtype->GetAddr());
        size_t offset = sp - symtype->GetAddr();
        os << "\tmovq %rsp, %rax\n";
        os << "\taddq $" << offset << ", %rax\n";
      }
      delete symtype;
      break;
    }
    case (Lex::TokenLabel::TSUB): {
      // negate the value
      this->LoadValueIntoReg(os, instr.tokens[3], X86Registers::AX);
      os << "\tneg %rax\n";
      break;
    }

    default: {
      fprintf(stderr, "syntax error\n");
      assert(0);
      break;
    }
    }

    this->StoreVarFromReg(os, instr.tokens[0].buf, X86Registers::AX);
    return os;
  }

  if (instr.tokens.size() == 5) {
    // example: a = foo + bar;
    this->LoadValueIntoReg(os, instr.tokens[2], X86Registers::AX);
    // load into R10, for it is callee owned
    this->LoadValueIntoReg(os, instr.tokens[4], X86Registers::R10);

    const char *r10 = X86Regs64Bit[static_cast<int>(X86Registers::R10)];
    const char *rax = X86Regs64Bit[static_cast<int>(X86Registers::AX)];

    // the operator
    switch (instr.tokens[3].label) {
    case (Lex::TokenLabel::TADD): {
      // add %r10, %rax
      os << "\tadd %" << r10 << ", %" << rax << "\n"; 
      break;
    }
    case (Lex::TokenLabel::TMUL): {
      // mul %r10, %rax
      os << "\tmul %" << r10 << ", %" << rax << "\n";
      break;
    }
    case (Lex::TokenLabel::TNE): {
      // example: a = b != c;
      // cmp $10, %rax
      // jne .L1
      // .L0:
      //  movb $0, %rax
      //  jmp .L2
      // .L1:
      //  movb $1, %rax
      // .L2:
      size_t true_label = this->branch_count++;
      size_t false_label = this->branch_count++;
      size_t end_label = this->branch_count++;
      os << "\tcmp %" << r10 << ", %" << rax << "\n";
      os << "\tjne .L" << true_label << "\n";
      os << ".L" << false_label << ":\n";
      os << "\tmovq $0, %" << rax << "\n";
      os << "\tjmp .L" << end_label << "\n";
      os << ".L" << true_label << ":\n";
      os << "\tmovq $1, %" << rax << "\n";
      os << ".L" << end_label << ":\n";
      break;
    }
    case (Lex::TokenLabel::TEQ): {
      // example: a = b == c;
      // cmp $10, %rax
      // je .L1
      // .L0:
      //  movb $0, %rax
      //  jmp .L2
      // .L1:
      //  movb $1, %rax
      // .L2:
      size_t true_label = this->branch_count++;
      size_t false_label = this->branch_count++;
      size_t end_label = this->branch_count++;
      os << "\tcmp %" << r10 << ", %" << rax << "\n";
      os << "\tje .L" << true_label << "\n";
      os << ".L" << false_label << ":\n";
      os << "\tmovq $0, %" << rax << "\n";
      os << "\tjmp .L" << end_label << "\n";
      os << ".L" << true_label << ":\n";
      os << "\tmovq $1, %" << rax << "\n";
      os << ".L" << end_label << ":\n";
      break;
    }
    case (Lex::TokenLabel::TGE): {
      // example: a = b > c;
      // cmp %r10, %rax
      // jg .L1
      // .L0:
      //   movb $0, %rax
      //   jmp .L2
      // .L1:
      //   movb $1, %rax
      // .L2:
      size_t true_label = this->branch_count++;
      size_t false_label = this->branch_count++;
      size_t end_label = this->branch_count++;
      os << "\tcmp %" << r10 << ", %" << rax << "\n";
      os << "\tjg .L" << true_label << "\n";
      os << ".L" << false_label << ":\n";
      os << "\tmovq $0, %" << rax << "\n";
      os << "\tjmp .L" << end_label << "\n";
      os << ".L" << true_label << ":\n";
      os << "\tmovq $1, %" << rax << "\n";
      os << ".L" << end_label << ":\n";
      break;
    }
    case (Lex::TokenLabel::TGEQ): {
      // example: a = b > c;
      // cmp %r10, %rax
      // jge .L1
      // .L0:
      //   movb $0, %rax
      //   jmp .L2
      // .L1:
      //   movb $1, %rax
      // .L2:
      size_t true_label = this->branch_count++;
      size_t false_label = this->branch_count++;
      size_t end_label = this->branch_count++;
      os << "\tcmp %" << r10 << ", %" << rax << "\n";
      os << "\tjge .L" << true_label << "\n";
      os << ".L" << false_label << ":\n";
      os << "\tmovq $0, %" << rax << "\n";
      os << "\tjmp .L" << end_label << "\n";
      os << ".L" << true_label << ":\n";
      os << "\tmovq $1, %" << rax << "\n";
      os << ".L" << end_label << ":\n";
      break;
    }
    case (Lex::TokenLabel::TLE): {
      // example: a = b < c;
      // cmp %r10, %rax
      // jl .L1
      // .L0:
      //   movb $0, %rax
      //   jmp .L2
      // .L1:
      //   movb $1, %rax
      // .L2:
      size_t true_label = this->branch_count++;
      size_t false_label = this->branch_count++;
      size_t end_label = this->branch_count++;
      os << "\tcmp %" << r10 << ", %" << rax << "\n";
      os << "\tjl .L" << true_label << "\n";
      os << ".L" << false_label << ":\n";
      os << "\tmovq $0, %" << rax << "\n";
      os << "\tjmp .L" << end_label << "\n";
      os << ".L" << true_label << ":\n";
      os << "\tmovq $1, %" << rax << "\n";
      os << ".L" << end_label << ":\n";
      break;
    }
    case (Lex::TokenLabel::TLEQ): {
      // example: a = b < c;
      // cmp %r10, %rax
      // jle .L1
      // .L0:
      //   movb $0, %rax
      //   jmp .L2
      // .L1:
      //   movb $1, %rax
      // .L2:
      size_t true_label = this->branch_count++;
      size_t false_label = this->branch_count++;
      size_t end_label = this->branch_count++;
      os << "\tcmp %" << r10 << ", %" << rax << "\n";
      os << "\tjle .L" << true_label << "\n";
      os << ".L" << false_label << ":\n";
      os << "\tmovq , %" << rax << "\n";
      os << "\tjmp .L" << end_label << "\n";
      os << ".L" << true_label << ":\n";
      os << "\tmovq $0, %" << rax << "\n";
      os << ".L" << end_label << ":\n";
      break;
    }

    default: {
      // FIXME
      throw std::runtime_error("Not implemented");
    }
    }

    // the result is in AX.
    this->StoreVarFromReg(os, instr.tokens[0].buf, X86Registers::AX);
    return os;
  }

  return os;
}

auto X86Generator::LoadValueIntoReg(std::ostringstream &os, const Lex::Token &token, X86Registers reg)
    -> std::ostringstream & {
  
  const char *reg_name = X86Regs64Bit[static_cast<int>(reg)];
  if (token.label == Lex::TokenLabel::TDOUBLEQUOTE) {
    const auto var_name = this->GetNameOfString(token.buf);
    // example: leaq var_name(%rip), %rax
    os << "\tleaq " << var_name << "(%rip), %" << reg_name << "\n";
    return os;
  }
  assert(token.label == Lex::TokenLabel::TALPHA);

  if (isdigit(token.buf[0])) {
    long val = Atoi(token.buf);
    os << "\tmovq $" << val << ", %" << reg_name << "\n";
  } else {
    this->LoadVarIntoReg(os, token.buf, reg);
  }

  return os;
}

auto X86Generator::GenerateCodeWithDebugInfo(Parser::BasicBlock *root) 
  -> std::string {
  throw std::runtime_error("Not implemented");
}

auto X86Generator::StoreArgsIntoMem(std::ostringstream &os, const Parser::Instruction &instr)
  -> std::ostringstream & {
  size_t i = 0;
  size_t nargs = 0;
  static const size_t max_args = 6;
  static const X86Registers function_args[] = {
    X86Registers::DI, // arg1: rdi,
    X86Registers::SI, // arg2: rsi
    X86Registers::DX, // arg3: rdx
    X86Registers::CX, // arg4: rcx
    X86Registers::R8, // arg5: r8
    X86Registers::R9, // arg6: r9
  };

  while (i < instr.tokens.size()) {
    if (instr.GetTypeOfToken(i) == Lex::TokenLabel::TLEFTPARENT) {
      break;
    }
    i++;
  }
  i++;
  assert(i < instr.tokens.size());

  while (instr.GetTypeOfToken(i) != Lex::TokenLabel::TRIGHTPARENT) {
    size_t j = i;
    SymbolType symtype;
    symtype.is_global = false;

    // base_type
    switch (instr.GetTypeOfToken(j)) {
    case (Lex::TokenLabel::TCHAR): {
      symtype.base_type = SymbolType::BaseType::TCHAR;
      break;
    }
    case (Lex::TokenLabel::TBOOL): {
      symtype.base_type = SymbolType::BaseType::TBOOL;
      break;
    }
    case (Lex::TokenLabel::TINT): {
      symtype.base_type = SymbolType::BaseType::TINT;
      break;
    }
    case (Lex::TokenLabel::TVOID): {
      symtype.base_type = SymbolType::BaseType::TVOID;
      break;
    }

    default: {
      fprintf(stderr, "unsupported type name %s\n", instr.tokens[i].buf.c_str());
      assert(0);
      break;
    }
    }

    // pointer level
    j++;
    while (instr.GetTypeOfToken(j) == Lex::TokenLabel::TMUL) {
      symtype.pointer_level++;
      j++;
    }

    // argument name
    assert(instr.GetTypeOfToken(j) == Lex::TokenLabel::TALPHA);
    auto stack_frame = this->symtab.GetCurrentStackFrame();
    symtype.stack_frame = stack_frame;
    size_t old_size = stack_frame->alloc_size;
    symtype.addr = stack_frame->Alloc(symtype.MemorySize());
    if (stack_frame->alloc_size > old_size) {
      os << "\taddq $-" << stack_frame->alloc_size - old_size << ", %rsp\n";
    }
    this->symtab.AddSymbol(instr.tokens[j].buf, symtype);
    assert(nargs < max_args);
    this->StoreVarFromReg(os, instr.tokens[j].buf, function_args[nargs++]);
    j++;

    i = j;
    assert(instr.GetTypeOfToken(i) == Lex::TokenLabel::TCOMMA ||
      instr.GetTypeOfToken(i) == Lex::TokenLabel::TRIGHTPARENT) ;
  }

  return os;
}

} // namespace Generator

namespace BugInsertor {

auto WrongOperator(const std::vector<Lex::Token> &src, size_t count)
  -> InsertionResult {
  InsertionResult ret;
  ret.first = false;
  return ret;

  // Replacement policy:
  // (==, !=) to (=)
  // (>, >=) as (<, <=) and vice versa
  // (++) as (--), and vice versa
  // (+=) as (-=), and vice versa
  // && as &
  // || as |
  // ! as ~, and vice versa

  // size_t num_insertion_point = 0;
  // size_t i = 0;
}

} // namespace BugInsertor
