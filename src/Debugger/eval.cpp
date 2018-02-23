#include <exception>
#include <utility>
#include <functional>
#include "common.h"
#include "Debugger.hpp"
#include "Debugger/regs.h"
#include "Debugger/expr.h"

typedef std::vector<token>::const_iterator token_iter;

static bool check_parentheses(token_iter l, token_iter r) {
  if (l->token_type != TK_LEFT_PARENTHESIS || 
      (r-1)->token_type != TK_RIGHT_PARENTHESIS)
    return false;
  int cnt = 1;
  for (auto it = l + 1; it < r - 1; it++) {
    if (it->token_type == TK_LEFT_PARENTHESIS) cnt++;
    else if (it->token_type == TK_RIGHT_PARENTHESIS) cnt--;
    if (cnt == 0) return false;
  }
  return true;
}

expr Debugger::build_expr(token_iter l, token_iter r) {
  if (l >= r) throw std::runtime_error("Invalid expression! #1");
  else if (l == r - 1) {
    switch (l->token_type) {
      case TK_NUMBER: {
        uint32_t num = l->value.u32;
        return [=] {return num;};
      }
      case TK_REGISTER: { 
        auto reg_info = get_reg_info(cpu, l->value.regid);
        void* addr = reg_info.first;
        switch (reg_info.second) {
          case SIZE_32: 
            return [=] {return *static_cast<uint32_t*>(addr);};
          case SIZE_16: 
            return [=] {return *static_cast<uint16_t*>(addr);};
          case SIZE_8:
            return [=] {return *static_cast<uint8_t*>(addr);};
          default:      panic("Unexpected register size!");
        }
      }
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
      else if (depth == 0 && (it->precedence() > precedence ||
            (it->precedence() == precedence && it->assoc_l2r()))) {
        precedence = it->precedence();
        cur = it;
      }
    }
    expr lhs, rhs;
    if (cur == r) throw std::runtime_error("Invalid expression! #3");
    switch (cur->token_type) {
      case TK_UNARY_PLUS:
        return build_expr(cur+1, r);
      case TK_UNARY_MINUS: 
        rhs = build_expr(cur+1, r);
        return [=] {return -rhs();}; 
      case TK_DEREFERENCE: {
        MMU& mmu = cpu.mmu;
        rhs = build_expr(cur+1, r);
        return [&mmu, rhs] {return mmu.vaddr_read<uint32_t>(rhs());};  
      }
      case TK_NOT: 
        rhs = build_expr(cur+1, r);
        return [=] {return !rhs();};
      case TK_PLUS:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() + rhs();};
      case TK_MINUS:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() - rhs();};
      case TK_MULTIPLICATION:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() * rhs();};
      case TK_DIVISION:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() / rhs();};
      case TK_EQUAL:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() == rhs();};
      case TK_INEQUAL:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() != rhs();};
      case TK_AND:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() && rhs();};
      case TK_OR:
        lhs = build_expr(l, cur); rhs = build_expr(cur+1, r);
        return [=] {return lhs() || rhs();};
      default: panic("Unexpected token type!");
    }
  }
  panic("Should not reach here!");
  return NULL;
}

