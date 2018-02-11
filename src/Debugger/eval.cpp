#include <exception>
#include <utility>
#include <functional>
#include "common.h"
#include "Debugger/regs.h"
#include "Debugger/expr.h"

namespace Evaluator {
  typedef std::function<uint32_t()> expr;
  typedef std::function<uint32_t(uint32_t)> unary_operator;
  typedef std::function<uint32_t(uint32_t, uint32_t)> binary_operator;

  uint32_t unary_composition(unary_operator op, expr rhs) {
    return op(rhs());
  }

  uint32_t binary_composition(binary_operator op, expr lhs, expr rhs) {
    return op(lhs(), rhs());
  }

  struct expr_number {
    uint32_t value;
    uint32_t operator()() {return value;}
  };

  struct expr_reg {
    uint16_t regid;
    uint32_t operator()() {return 0; /*TODO*/}    
  };
  
  typedef std::vector<token>::const_iterator token_iter;

  static bool check_parentheses(token_iter l, token_iter r) {
    if (l->token_type != TK_LEFT_PARENTHESIS || (r-1)->token_type != TK_RIGHT_PARENTHESIS)
      return false;
    int cnt = 1;
    for (auto it = l + 1; it < r - 1; it++) {
      if (it->token_type == TK_LEFT_PARENTHESIS) cnt++;
      else if (it->token_type == TK_RIGHT_PARENTHESIS) cnt--;
      if (cnt == 0) return false;
    }
    return true;
  }

  expr build_expr(token_iter l, token_iter r) {
//  printf("dist = %ld\n", r - l);
    if (l >= r) throw std::runtime_error("Invalid expression! #1");
    else if (l == r - 1) {
      switch (l->token_type) {
        case TK_NUMBER: return expr_number({l->value.u32});
        case TK_REGISTER: return expr_reg({l->value.regid});
        default: throw std::runtime_error("Invalid expression! #2");
      }
    } else if (check_parentheses(l, r)) return build_expr(l+1, r-1);
    else {
      uint8_t precedence = 0;
      int depth = 0;
      token_iter cur = r;
      for (auto it = l; it < r; it++) {
        if (it->token_type == TK_LEFT_PARENTHESIS) depth++;
        else if (it->token_type == TK_RIGHT_PARENTHESIS) depth--;
        else if (depth == 0 && it->precedence() >= precedence) {
          precedence = it->precedence();
          cur = it;
        }
      }
      if (cur == r) throw std::runtime_error("Invalid expression! #3");
      switch (cur->token_type) {
        case TK_UNARY_PLUS:
          return build_expr(cur+1, r);
        case TK_UNARY_MINUS:
          return std::bind(unary_composition, 
            std::negate<uint32_t>(), build_expr(cur+1, r));
        case TK_DEREFERENCE:
          panic("unimplemented!");
        case TK_NOT:
          return std::bind(unary_composition,
            std::logical_not<uint32_t>(), build_expr(cur+1, r));
        case TK_PLUS:
          return std::bind(binary_composition, std::plus<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_MINUS:
          return std::bind(binary_composition, std::minus<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_MULTIPLICATION:
          return std::bind(binary_composition, std::multiplies<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_DIVISION:
          return std::bind(binary_composition, std::divides<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_EQUAL:
          return std::bind(binary_composition, std::equal_to<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_INEQUAL:
          return std::bind(binary_composition, std::not_equal_to<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_AND:
          return std::bind(binary_composition, std::logical_and<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        case TK_OR:
          return std::bind(binary_composition, std::logical_or<uint32_t>(),
            build_expr(l, cur), build_expr(cur+1, r));
        default: panic("Unexpected token type!");
      }
    }
    panic("Should not reach here!");
  }
};

