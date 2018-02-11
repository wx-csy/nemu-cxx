#include <vector>
#include <functional>
#include "common.h"
#include "Debugger/regs.h"

namespace Evaluator {
  enum TOKEN_TYPE : uint16_t {
    TK_NUMBER = 0x0000, TK_REGISTER,
    TK_LEFT_PARENTHESIS = 0xff00, TK_RIGHT_PARENTHESIS,
    TK_UNARY_PLUS = 0x0100, TK_UNARY_MINUS, TK_DEREFERENCE, TK_NOT,
    TK_MULTIPLICATION = 0x0200, TK_DIVISION,
    TK_PLUS = 0x0300, TK_MINUS,
    TK_EQUAL = 0x0400, TK_INEQUAL,
    TK_AND = 0x0500, TK_OR = 0x0600, 
  };

  struct token {
    TOKEN_TYPE token_type;
    
    union {
      uint32_t u32;
      uint16_t regid;
    } value;

    uint8_t precedence() const {return token_type >> 8;}
  };

  std::vector<token> tokenize(const char* expr);

  typedef std::function<uint32_t()> expr;
  typedef std::vector<token>::const_iterator token_iter;

  expr build_expr(token_iter l, token_iter r);
};

