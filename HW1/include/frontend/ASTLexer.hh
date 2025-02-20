#ifndef _ASTLexer_hh
#define _ASTLexer_hh

#include <fstream>
#include <iostream>
#include <string>
#if !defined(yyFlexLexerOnce)
#define yyFlexLexer yy_ast_FlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif
#include "FDMJAST.hh"
#include "ast_location.hh"

using namespace std;
using namespace fdmj;

namespace fdmj {
// These are the types of values (yylval) that a token can have
// This can change if more types are added
class AST_YYSTYPE {
public:
  int i;
  string s;
  IntExp *intExp;
  IdExp *idExp;
  OpExp *opExp;
  Program *program;
  MainMethod *mainMethod;
  Stm *stm;
  vector<Stm *> *stmList;
  Exp *exp;

  Program *root;
};

class ASTLexer : public yy_ast_FlexLexer {
  // initialize the line and column numbers.
  // Each time a newline is encountered, the line number should increment and
  // column reset
  std::size_t currentLine = 1;
  std::size_t currentColumn = 1;

  // These are the values that the lexer will return
  AST_YYSTYPE *yylval = nullptr;
  location_t *yylloc = nullptr;

  // This is the function that will copy the value of the token to yylval
  // The lexer will call this function to copy the value of the token to yylval
  // We will use it when a token is matched and need to return a value in
  // yylval.
  void copyValue(const string s) { yylval->s = s; }
  void copyValue(const int n) { yylval->i = n; }
  // void copyValue(const ASSTNode *node) { yylval->node = nullptr;
  // exit(EXIT_FAILURE);} //the lexer shouldnt see node!

  // this is the function that will copy the location of the token to yylloc
  // this is needed in order to keep track of the location of the token
  // we will use it for YY_USER_ACTION, i.e., whenever a pattern in matched
  void copyLocation() {
    *yylloc = location_t(currentLine, currentColumn, currentLine,
                         yyleng + currentColumn - 1);
    currentColumn += yyleng;
  }

public:
  // The API to this lexer
  ASTLexer(std::istream &in, const bool debug) : yy_ast_FlexLexer(&in) {
    yy_ast_FlexLexer::set_debug(debug);
  }
  int yylex(AST_YYSTYPE *const lval, location_t *const lloc);
};
} // namespace fdmj
#endif