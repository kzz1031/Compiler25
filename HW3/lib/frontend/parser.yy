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
%token CLASS EXTENDS
%token IF ELSE WHILE BREAK CONTINUE THIS NEW LENGTH
%token TRUE FALSE PUTINT PUTCH PUTARRAY GETINT GETCH GETARRAY
%token STARTTIME STOPTIME NOT
//terminals with value
%token<i> NONNEGATIVEINT
%token<s> IDENTIFIER
%token '(' ')' '[' ']' '{' '}' '=' ',' ';' '.' 
%token ADD MINUS TIMES DIVIDE EQ NE LT LE GT GE AND OR
//non-terminals, need type information only (not tokens)
%type <program> PROG 
%type <mainMethod> MAINMETHOD 
%type <stm> STM
%type <stmList> STM_LIST
%type <exp> EXP
%type <idExp> ID 
%type <type> TYPE
%type <classDeclList> CLASS_DECL_LIST
%type <classDecl> CLASS_DECL
%type <methodDeclList> METHOD_DECL_LIST
%type <methodDecl> METHOD_DECL
%type <varDeclList> VAR_DECL_LIST
%type <varDecl> VAR_DECL
%type <formalList> FORMAL_LIST FORMAL_REST
%type <intExp> CONST
%type <expList> EXP_LIST EXP_REST
%type <intExpList> CONST_LIST CONST_REST
// 运算符优先级和结合性
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left ADD MINUS
%left TIMES DIVIDE
%right UMINUS NOT
%left '.' '[' '('

// if-else 优先级
%nonassoc THEN
%nonassoc ELSE

%start PROG
%expect 0

%%
PROG: MAINMETHOD CLASS_DECL_LIST
  { 
#ifdef DEBUG
    cerr << "Program" << endl;
#endif
    result->root = new Program(p, $1, $2);
    $$ = result->root;
  }
  ;
MAINMETHOD: PUBLIC INT MAIN '(' ')' '{' VAR_DECL_LIST STM_LIST '}'
  {
#ifdef DEBUG
    cerr << "MainMethod" << endl;
#endif
    $$ = new MainMethod(p, $7, $8) ;
  }
  ;

STM_LIST: /* empty */
  {
    $$ = new vector<Stm*>();
  }
  | STM STM_LIST
  {
    vector<Stm*> *v = $2;
    v->push_back($1);
    rotate(v->begin(), v->end() - 1, v->end());
    $$ = v;
  }
  ;

STM: '{' STM_LIST '}'
  {
    $$ = new Nested(p, $2);
  }
  | IF '(' EXP ')' STM ELSE STM
  {
    $$ = new If(p, $3, $5, $7);
  }
  | IF '(' EXP ')' STM %prec THEN
  {
    $$ = new If(p, $3, $5, nullptr);
  }
  | WHILE '(' EXP ')' STM
  {
    $$ = new While(p, $3, $5);
  }
  | WHILE '(' EXP ')' ';'
  {
    $$ = new While(p, $3, nullptr);
  }
  | EXP '=' EXP ';'
  {
    $$ = new Assign(p, $1, $3);
  }
  | EXP '.' ID '(' EXP_LIST ')' ';'
  {
    $$ = new CallStm(p, $1, $3, $5);
  }
  | CONTINUE ';'
  {
    $$ = new Continue(p);
  }
  | BREAK ';'
  {
    $$ = new Break(p);
  }
  | RETURN EXP ';'
  {
    $$ = new Return(p, $2);
  }
  | PUTINT '(' EXP ')' ';'
  {
    $$ = new PutInt(p, $3);
  }
  | PUTCH '(' EXP ')' ';'
  {
    $$ = new PutCh(p, $3);
  }
  | PUTARRAY '(' EXP ',' EXP ')' ';'
  {
    $$ = new PutArray(p, $3, $5);
  }
  | STARTTIME '(' ')' ';'
  {
    $$ = new Starttime(p);
  }
  | STOPTIME '(' ')' ';'
  {
    $$ = new Stoptime(p);
  }
  | error ';'
  {
    yyerrok;
    $$ = nullptr;
  }
  ;
  | error '}'
  {
    yyerrok;
    $$ = nullptr;
  }
  ;
  
CLASS_DECL_LIST: /* empty */
  {
    $$ = nullptr;
  }
  | CLASS_DECL CLASS_DECL_LIST
  {
    if ($1 != nullptr) {
      vector<ClassDecl*>* v = new vector<ClassDecl*>();
      v->push_back($1);
      if ($2 != nullptr) {
        v->insert(v->end(), $2->begin(), $2->end());
      }
      $$ = v;
    } else {
      $$ = $2;
    }
  }
  ;

CLASS_DECL: PUBLIC CLASS ID '{' VAR_DECL_LIST METHOD_DECL_LIST '}'
  {
    $$ = new ClassDecl(p, $3, nullptr, $5, $6);
  }
  | PUBLIC CLASS ID EXTENDS ID '{' VAR_DECL_LIST METHOD_DECL_LIST '}'
  {
    $$ = new ClassDecl(p, $3, $5, $7, $8);
  }
  |
  error '}'
  {
    yyerrok;
    $$ = nullptr;
  }//error recovery
  ;

VAR_DECL_LIST: /* empty */
  {
    $$ = nullptr;
  }
  | VAR_DECL VAR_DECL_LIST
  {
    if ($1 != nullptr) {
      vector<VarDecl*>* v = new vector<VarDecl*>();
      v->push_back($1);
      if ($2 != nullptr) {
        v->insert(v->end(), $2->begin(), $2->end());
      }
      $$ = v;
    } else {
      $$ = $2;
    }
  }
  ;
//TODO
VAR_DECL: CLASS ID ID ';'
  {
    Pos *pos = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new VarDecl(p, new Type(pos, $2), $3);
  }
  | INT ID ';'
  {
    Pos *pos = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new VarDecl(p, new Type(pos), $2);
  }
  | INT ID '=' CONST ';'
  {
    Pos *pos = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    Pos *p1 = new Pos(@4.sline, @4.scolumn, @4.eline, @4.ecolumn);
    $$ = new VarDecl(p, new Type(pos), $2, $4);
  }
  | INT '[' ']' ID ';'
  {
    Pos *pos = new Pos(@4.sline, @4.scolumn, @4.eline, @4.ecolumn);
    IntExp* exp = new IntExp(p, 0);
    $$ = new VarDecl(p, new Type(pos, exp), $4);
  }
  | INT '[' ']' ID '=' '{' CONST_LIST '}' ';'
  {
    Pos *pos = new Pos(@4.sline, @4.scolumn, @4.eline, @4.ecolumn);
    IntExp* exp = new IntExp(p, 0);
    $$ = new VarDecl(p, new Type(pos, exp), $4, $7);
  }
  | INT '[' NONNEGATIVEINT ']' ID ';'
  {
    Pos *pos = new Pos(@5.sline, @5.scolumn, @5.eline, @5.ecolumn);
    IntExp* exp = new IntExp(pos, $3);
    $$ = new VarDecl(p, new Type(pos, exp), $5);  }
  | INT '[' NONNEGATIVEINT ']' ID '=' '{' CONST_LIST '}' ';'
  {
    Pos *pos = new Pos(@5.sline, @5.scolumn, @5.eline, @5.ecolumn);
    IntExp* exp = new IntExp(pos, $3);
    $$ = new VarDecl(p, new Type(pos, exp), $5, $8);
  }
  | INT error ';'
  {
    yyerrok;
    $$ = nullptr;
  }
  | CLASS error ';'
  {
    yyerrok;
    $$ = nullptr;
  }
  ;

METHOD_DECL_LIST: /* empty */
  {
    $$ = nullptr;
  }
  | METHOD_DECL METHOD_DECL_LIST
  {
    if ($1 != nullptr) {
      vector<MethodDecl*>* v = new vector<MethodDecl*>();
      v->push_back($1);
      if ($2 != nullptr) {
        v->insert(v->end(), $2->begin(), $2->end());
      }
      $$ = v;
    } else {
      $$ = $2;
    }
  }
  ;

METHOD_DECL: PUBLIC TYPE ID '(' FORMAL_LIST ')' '{' VAR_DECL_LIST STM_LIST '}'
  {
#ifdef DEBUG
    cerr << "MethodDecl" << endl;
#endif
    $$ = new MethodDecl(p, $2, $3, $5, $8, $9);
  }
  | error '}'
  {
    yyerrok;
    $$ = nullptr;
  }
  ;

FORMAL_LIST: /* empty */
  {
    $$ = nullptr;
  }
  | TYPE ID FORMAL_REST
  {
    vector<Formal*>* v = new vector<Formal*>();
    Pos* pos = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    v->push_back(new Formal(pos, $1, $2));
    if ($3 != nullptr) {
      v->insert(v->end(), $3->begin(), $3->end());
    }
    $$ = v;
  }
  ;

FORMAL_REST: /* empty */
  {
    $$ = nullptr;
  }
  |
  ',' TYPE ID FORMAL_REST
  {
    vector<Formal*>* v = new vector<Formal*>();
    Pos* pos = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    v->push_back(new Formal(pos, $2, $3));
    if ($4 != nullptr) {
      v->insert(v->end(), $4->begin(), $4->end());
    }
    $$ = v;
  }
  ;

EXP: EXP ADD EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "+"), $3);
  }
  | EXP MINUS EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "-"), $3);
  }
  | EXP TIMES EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "*"), $3);
  }
  | EXP DIVIDE EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "/"), $3);
  }
  | EXP AND EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "&&"), $3);
  }
  | EXP OR EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "||"), $3);
  }
  | EXP LT EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "<"), $3);
  }
  | EXP LE EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "<="), $3);
  }
  | EXP GT EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, ">"), $3);
  }
  | EXP GE EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, ">="), $3);
  }
  | EXP EQ EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "=="), $3);
  }
  | EXP NE EXP
  {
    Pos *p1 = new Pos(@2.sline, @2.scolumn, @2.eline, @2.ecolumn);
    $$ = new BinaryOp(p, $1, new OpExp(p1, "!="), $3);
  }
  | NOT EXP
  {
    Pos *p1 = new Pos(@1.sline, @1.scolumn, @1.eline, @1.ecolumn);
    $$ = new UnaryOp(p, new OpExp(p1, "!"), $2);
  }
  | MINUS EXP %prec UMINUS
  {
    Pos *p1 = new Pos(@1.sline, @1.scolumn, @1.eline, @1.ecolumn);
    $$ = new UnaryOp(p, new OpExp(p1, "-"), $2);
  }
  | '(' EXP ')'
  {
    $$ = $2;
  }
  | NONNEGATIVEINT
  {
    $$ = new IntExp(p, $1);
  }
  | TRUE
  {
    $$ = new BoolExp(p, true);
  }
  | FALSE
  {
    $$ = new BoolExp(p, false);
  }
  | THIS
  {
    $$ = new This(p);
  }
  | LENGTH '(' EXP ')'
  {
    $$ = new Length(p, $3);
  }
  | GETINT '(' ')'
  {
    $$ = new GetInt(p);
  }
  | GETCH '(' ')'
  {
    $$ = new GetCh(p);
  }
  | GETARRAY '(' EXP ')'
  {
    $$ = new GetArray(p, $3);
  }
  | ID
  {
    $$ = $1;
  }
  | EXP '.' ID
  {
    $$ = new ClassVar(p, $1, $3);
  }
  | EXP '[' EXP ']'
  {
    $$ = new ArrayExp(p, $1, $3);
  }
  | EXP '.' ID '(' EXP_LIST ')'
  {
    $$ = new CallExp(p, $1, $3, $5);
  }
  | '(' '{' STM_LIST'}' EXP ')'
  {
    $$ = new Esc(p, $3, $5);
  }
  ;

EXP_LIST: /* empty */
  {
    $$ = nullptr;
  }
  | EXP EXP_REST
  {
    vector<Exp*>* v = new vector<Exp*>();
    v->push_back($1);
    if ($2 != nullptr) {
      v->insert(v->end(), $2->begin(), $2->end());
    }
    $$ = v;
  }
  ;

EXP_REST: /* empty */
  {
    $$ = nullptr;
  }
  | ',' EXP EXP_REST
  {
    vector<Exp*>* v = new vector<Exp*>();
    v->push_back($2);
    if ($3 != nullptr) {
      v->insert(v->end(), $3->begin(), $3->end());
    }
    $$ = v;
  }
  ;

CONST_LIST: /* empty */
  {
    $$ = nullptr;
  }
  | CONST CONST_REST
  {
    vector<IntExp*>* v = new vector<IntExp*>();
    v->push_back($1);
    if ($2 != nullptr) {
      v->insert(v->end(), $2->begin(), $2->end());
    }
    $$ = v;
  }
  ;

CONST: NONNEGATIVEINT
  {
    $$ = new IntExp(p, $1);
  }
  | MINUS NONNEGATIVEINT
  {
    $$ = new IntExp(p, -$2);
  }
  ;

CONST_REST: /* empty */
  {
    $$ = nullptr;
  }
  | ',' CONST CONST_REST
  {
    vector<IntExp*>* v = new vector<IntExp*>();
    v->push_back($2);
    if ($3 != nullptr) {
      v->insert(v->end(), $3->begin(), $3->end());
    }
    $$ = v;
  }
  ;

ID: IDENTIFIER
  {
    $$ = new IdExp(p, $1);
  }
  ;

TYPE: CLASS ID
  {
    $$ = new Type(p, $2);
  }
  | INT '[' ']'
  {
    IntExp* exp = new IntExp(p, 0);
    $$ = new Type(p, exp);
  }
  | INT
  {
    $$ = new Type(p);
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
