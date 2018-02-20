#include <exception>
#include <vector>
#include <cctype>
#include "common.h"
#include "Debugger/expr.h"
#include "Debugger/regs.h"

  struct Tokenizer {
    std::vector<token> tokens;

    const char* expr;

    void error() {
      throw std::runtime_error("Unexpected token!");
    }

    char peek() {
      return *expr; 
    };
    
    char get() {
      return *(expr++);
    }

    bool expect(char ch) {
      if (*expr == ch) {
        expr++;
        return true;
      } else {
        return false;
      }
    }
    
    void Expect(char ch) {
      if (*expr == ch) expr++; else error();
    }
    
    void retreat(int n = 1) {
      expr -= n;
    }
    
    bool is_unary() {
      return tokens.empty() || 
        (tokens.back().token_type != TK_NUMBER &&
         tokens.back().token_type != TK_REGISTER &&
         tokens.back().token_type != TK_RIGHT_PARENTHESIS);
    }   

    void get_number(const char *pattern) {
      uint32_t value;
      int n;
      if (sscanf(expr, pattern, &value, &n)){
        expr += n;
        tokens.push_back({TK_NUMBER, value});
      } else error();
    }

    void get_register() {
      const char* cur = expr;
      while (isalpha((unsigned char)(peek()))) get();
      auto it = regid.find(std::string(cur, expr-cur));
      if (it != regid.end()) {
        tokens.push_back({TK_REGISTER, it->second});
      } else {
        error();
      }
    }

    Tokenizer(const char* _expr) {
      expr = _expr;

      while (true) {
        switch (get()) {
          case ' ': break;
          case '0': if (expect('x') || expect('X')) {
                      get_number("%x%n"); 
                    } else {
                      retreat(); get_number("%u%n");
                    } break;
          // case ranges, a gcc extension
          case '1' ... '9': retreat(); get_number("%u%n"); break;
          case '$': get_register(); break;
          case '(': tokens.push_back({TK_LEFT_PARENTHESIS}); break;
          case ')': tokens.push_back({TK_RIGHT_PARENTHESIS}); break;
          case '+': tokens.push_back({is_unary() ? TK_UNARY_PLUS : TK_PLUS}); break;
          case '-': tokens.push_back({is_unary() ? TK_UNARY_MINUS : TK_MINUS}); break;
          case '*': tokens.push_back({is_unary() ? TK_DEREFERENCE : TK_MULTIPLICATION});
                      break;
          case '/': tokens.push_back({TK_DIVISION}); break;
          case '=': Expect('='); tokens.push_back({TK_EQUAL}); break;
          case '&': Expect('&'); tokens.push_back({TK_AND}); break;
          case '|': Expect('|'); tokens.push_back({TK_OR}); break;
          case '!': if (expect('=')) {
                      tokens.push_back({TK_INEQUAL});
                    } else {
                      tokens.push_back({TK_NOT});
                    } break;
          case 0  : return;
          default : throw std::runtime_error("Unexpected token!");
        }
      } 
    }
  };

  std::vector<token> tokenize(const char* expr) {
    Tokenizer tokens(expr);
//    for (auto tk : tokens.tokens) printf("%d\n", tk.token_type);
    return tokens.tokens;
  }

