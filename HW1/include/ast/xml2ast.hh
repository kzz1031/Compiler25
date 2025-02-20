#ifndef _XML2AST_HH
#define _XML2AST_HH

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

// create a node from an XML node, which must match the given node type
// The initial call should be with the root non-terminal (usually Program) and
// the root of the XML tree (usually tag: "Program")
Program *xml2ast(string xmlfilename);
Program *xml2ast(XMLElement *element);

// forward declarations
Program *create_program(XMLElement *);
MainMethod *create_mainMethod(XMLElement *);
Stm *create_stm(XMLElement *);
Assign *create_assign(XMLElement *);
Return *create_return(XMLElement *);
Exp *create_exp(XMLElement *);
BinaryOp *create_binaryOp(XMLElement *);
UnaryOp *create_unaryOp(XMLElement *);
IdExp *create_idExp(XMLElement *);
IntExp *create_intexp(XMLElement *);
OpExp *create_opexp(XMLElement *);
#endif