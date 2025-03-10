%skeleton "lalr1.cc"
%require  "3.2"
%language "c++"
%locations

%code requires {
  #define DEBUG
  #undef DEBUG
  #include <iostream>
  #include <vector>
  #include <algorithm>
  #include <string>
  #include <variant>
  #include "ast_location.hh"
  #include "ASTLexer.hh"
  #include "ASTheader.hh"
  #include "FDMJAST.hh"

  using namespace std;
  using namespace fdmj;
}

%define api.namespace {fdmj}
%define api.parser.class {ASTParser}
%define api.value.type {AST_YYSTYPE}
%define api.location.type {ast_location}

%define parse.error detailed
%define parse.trace

%header
%verbose

%parse-param {ASTLexer &lexer}
%parse-param {const bool debug}
%parse-param {AST_YYSTYPE* result}

%initial-action
{
    #if YYDEBUG != 0
        set_debug_level(debug);
    #endif
};

%code {
    namespace fdmj 
    {
        template<typename RHS>
        void calcLocation(location_t &current, const RHS &rhs, const std::size_t n);
    }
    
    #define YYLLOC_DEFAULT(Cur, Rhs, N) calcLocation(Cur, Rhs, N)
    #define yylex lexer.yylex
    Pos *p;
}

//terminals with no value 
%token PUBLIC INT MAIN RETURN 
//terminals with value
%token<i> NONNEGATIVEINT
%token<s> IDENTIFIER
%token '(' ')' '[' ']' '{' '}' '=' ',' ';' '.' 
%token ADD MINUS TIMES DIVIDE EQ NE LT LE GT GE AND OR
//non-terminals, need type information only (not tokens)
%type <program> PROG 
%type <mainMethod> MAINMETHOD 
%type <stm> STM
%type <stmList> STMLIST
%type <exp> EXP
%type <idExp> ID 

%start PROG
%expect 0

%%
PROG: MAINMETHOD 
  { 
#ifdef DEBUG
    cerr << "Program" << endl;
#endif
    result->root = new Program(p, $1);
    //$$ = result->root;
  }
  ;
MAINMETHOD: PUBLIC INT MAIN '(' ')' '{' STMLIST '}'
  {
#ifdef DEBUG
    cerr << "MainMethod" << endl;
#endif
    $$ = new MainMethod(p, nullptr, $7) ;
  }
  ;
STMLIST: // empty
  {
#ifdef DEBUG
    cerr << "STMLIST empty" << endl;
#endif
    $$ = new vector<Stm*>();
  }
  |
  STM STMLIST
  {
#ifdef DEBUG
    cerr << "STM STMLIST" << endl;
#endif
    vector<Stm*> *v = $2;
    v->push_back($1);
    rotate(v->begin(), v->end() - 1, v->end());
    $$ = v;
  }
  ;
STM: ID '=' EXP ';'
  {
#ifdef DEBUG
    cerr << "Assign" << endl;
#endif
    $$ = new Assign(p, $1, $3);
  }
  |
  RETURN EXP ';'
  {
#ifdef DEBUG
    cerr << "Return" << endl;
#endif
    $$ = new Return(p, $2);
  }
  ;
EXP: '(' EXP ADD EXP ')'
  {
#ifdef DEBUG
    cerr << "EXP ADD EXP" << endl;
#endif
    Pos *p1 = new Pos(@3.sline, @3.scolumn, @3.eline, @3.ecolumn);
    $$ = new BinaryOp(p, $2, new OpExp(p1, "+"), $4);
  }
  |
  '(' EXP MINUS EXP ')'
  {
#ifdef DEBUG
    cerr << "EXP MINUS EXP" << endl;
#endif
    Pos *p1 = new Pos(@3.sline, @3.scolumn, @3.eline, @3.ecolumn);
    $$ = new BinaryOp(p, $2, new OpExp(p1, "-"), $4);
  }
  |
  '(' EXP TIMES EXP ')'
  {
#ifdef DEBUG
    cerr << "EXP TIMES EXP" << endl;
#endif
    Pos *p1 = new Pos(@3.sline, @3.scolumn, @3.eline, @3.ecolumn);
    $$ = new BinaryOp(p, $2, new OpExp(p1, "*"), $4);
  }
  |
  '(' EXP DIVIDE EXP ')'
  {
#ifdef DEBUG
    cerr << "EXP DIVIDE EXP" << endl;
#endif
    Pos *p1 = new Pos(@3.sline, @3.scolumn, @3.eline, @3.ecolumn);
    $$ = new BinaryOp(p, $2, new OpExp(p1, "/"), $4);
  }
  |
  NONNEGATIVEINT
  {
#ifdef DEBUG
    cerr << "NonNegativeInt: " << $1 << endl;
#endif
    $$ = new IntExp(p, $1);
  }
  |
  '(' MINUS EXP ')'
  {
#ifdef DEBUG
    cerr << "- EXP" << endl;
#endif
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ =  new UnaryOp(p, new OpExp(p1, "-"), $3);
  }
  |
  '(' EXP ')'
  {
#ifdef DEBUG
    cerr << "( EXP )" << endl;
#endif
    $$ = $2;
  }
  |
  '(' '{' STMLIST '}' EXP ')'
  {
#ifdef DEBUG
    cerr << "( { STMLIST } EXP )" << endl;
#endif
    $$ = new Esc(p, $3, $5);
  }
  |
  ID
  {
#ifdef DEBUG
    cerr << "ID" << endl;
#endif
    $$ = $1;
  }
  ;
ID: IDENTIFIER
  {
#ifdef DEBUG
    cerr << "Identifier: " << $1 << endl;
#endif
    $$ = new IdExp(p, $1);
  }
  ;

%%
/*
void yyerror(char *s) {
  fprintf(stderr, "%s\n",s);
}

int yywrap() {
  return(1);
}
*/

//%code 
namespace fdmj 
{
    template<typename RHS>
    inline void calcLocation(location_t &current, const RHS &rhs, const std::size_t n)
    {
        current = location_t(YYRHSLOC(rhs, 1).sline, YYRHSLOC(rhs, 1).scolumn, 
                                    YYRHSLOC(rhs, n).eline, YYRHSLOC(rhs, n).ecolumn);
        p = new Pos(current.sline, current.scolumn, current.eline, current.ecolumn);
    }
    
    void ASTParser::error(const location_t &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }

  Program* fdmjParser(ifstream &fp, const bool debug) {
    fdmj:AST_YYSTYPE result; 
    result.root = nullptr;
    fdmj::ASTLexer lexer(fp, debug);
    fdmj::ASTParser parser(lexer, debug, &result); //set up the parser
    if (parser() ) { //call the parser
      cout << "Error: parsing failed" << endl;
      return nullptr;
    }
    if (debug) cout << "Parsing successful" << endl;
    return result.root;
  }

  Program*  fdmjParser(const string &filename, const bool debug) {
    std::ifstream fp(filename);
    if (!fp) {
      cout << "Error: cannot open file " << filename << endl;
      return nullptr;
    }
    return fdmjParser(fp, debug);
  }
}
