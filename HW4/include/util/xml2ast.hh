#ifndef _XML2AST_HH
#define _XML2AST_HH

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "semant.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

//create a node from an XML node, which must match the given node type
//The initial call should be with the root non-terminal (usually Program) and the root of the XML tree (usually tag: "Program")
fdmj::Program* xml2ast(string xmlfilename, AST_Semant_Map **sm);
fdmj::Program* xml2ast(XMLElement* element, AST_Semant_Map **sm);

//forward declarations
fdmj::Program* create_program(XMLElement*);
fdmj::MainMethod* create_mainMethod(XMLElement*);
fdmj::VarDecl* create_varDecl(XMLElement*);
fdmj::MethodDecl* create_methodDecl(XMLElement*);
fdmj::ClassDecl* create_classDecl(XMLElement*);
fdmj::Type* create_type(XMLElement*);
fdmj::Formal* create_formal(XMLElement*);
fdmj::Stm* create_stm(XMLElement*);
fdmj::Nested* create_nested(XMLElement*);
fdmj::If* create_if(XMLElement*);
fdmj::While* create_while(XMLElement*);
fdmj::Assign* create_assign(XMLElement*);
fdmj::CallStm* create_callStm(XMLElement*);
fdmj::Continue* create_continue(XMLElement*);
fdmj::Break* create_break(XMLElement*);
fdmj::Return* create_return(XMLElement*);
fdmj::PutInt* create_putInt(XMLElement*);
fdmj::PutCh* create_putCh(XMLElement*);
fdmj::PutArray* create_putArray(XMLElement*);
fdmj::Starttime* create_starttime(XMLElement*);
fdmj::Stoptime* create_stoptime(XMLElement*);
fdmj::Exp* create_exp(XMLElement*);
fdmj::BinaryOp* create_binaryOp(XMLElement*);
fdmj::UnaryOp* create_unaryOp(XMLElement*);
fdmj::ArrayExp* create_arrayExp(XMLElement*);
fdmj::CallExp* create_callExp(XMLElement*);
fdmj::ClassVar* create_classVar(XMLElement*);
fdmj::BoolExp* create_bool(XMLElement*);
fdmj::This* create_this(XMLElement*);
fdmj::Length* create_length(XMLElement*);
fdmj::Esc* create_esc(XMLElement*);
fdmj::GetInt* create_getInt(XMLElement*);
fdmj::GetCh* create_getCh(XMLElement*);
fdmj::GetArray* create_getArray(XMLElement*);
fdmj::IdExp* create_idExp(XMLElement*);
fdmj::IntExp* create_intexp(XMLElement*);
fdmj::OpExp* create_opexp(XMLElement*);
#endif