// This file is used to convert AST to XML

#define DEBUG
#undef DEBUG

#include "xml2ast.hh"
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "tinyxml2.hh"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <variant>

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

#define ClassDeclList vector<ClassDecl *>
#define VarDeclList vector<VarDecl *>
#define MethodDeclList vector<MethodDecl *>
#define FormalList vector<Formal *>
#define StmList vector<Stm *>
#define ExpList vector<Exp *>

Program *xml2ast(XMLElement *element) {
  if (string(element->Name()) != "Program") {
    cerr << "Error: Root element is not Program" << endl;
    return nullptr;
  }
  return create_program(element);
}

Program *xml2ast(string xmlfilename) {
  XMLDocument doc;
  if (doc.LoadFile(xmlfilename.c_str()) != XML_SUCCESS) {
    cerr << "Error: " << xmlfilename << " not found" << endl;
    return nullptr;
  }
  if (doc.ErrorID() != XML_SUCCESS) {
    cerr << "Error: " << xmlfilename << " is not a valid XML file" << endl;
    return nullptr;
  }
  return xml2ast(doc.RootElement());
}

static Pos *get_position(XMLElement *element) {
#ifdef DEBUG
  // cout << "Getting position" << endl;
#endif
  int bline = 0, bpos = 0, eline = 0, epos = 0;
  const XMLAttribute *attr = element->FirstAttribute();
  while (attr) {
    string name = attr->Name();
    if (name == "bline") {
      bline = stoi(string(attr->Value()));
    } else if (name == "bpos") {
      bpos = stoi(string(attr->Value()));
    } else if (name == "eline") {
      eline = stoi(string(attr->Value()));
    } else if (name == "epos") {
      epos = stoi(string(attr->Value()));
    }
    attr = attr->Next();
  }
  // the XML file may not have positions info in it
  /*if (bline == 0 || bpos == 0 || eline == 0 || epos == 0) {
      //cerr << "Error: No position found in " << string(element->Name()) << ",
     or position is ill formed" << endl; return nullptr;
  */
  return new Pos(bline, bpos, eline, epos);
}

// forward declarations
Program *create_program(XMLElement *);
MainMethod *create_mainMethod(XMLElement *);
Assign *create_assign(XMLElement *);
Return *create_return(XMLElement *);
BinaryOp *create_binaryOp(XMLElement *);
UnaryOp *create_unaryOp(XMLElement *);
Esc *create_esc(XMLElement *);
// IdExp* create_idExp(XMLElement*);
// IntExp* create_intExp(XMLElement*);
// OpExp* create_opExp(XMLElement*);

ExpList *create_exp_list(XMLElement *);
StmList *create_stm_list(XMLElement *);

enum class ATTR_TYPE { INT, ID, OP };
template <class T>
T *create_leafnode(XMLElement *, string, string,
                   ATTR_TYPE); // forward declaration

template <class T> // for IntExp lists (can be expanded for other types)
vector<T *> *create_list(XMLElement *element, string tag) {
#ifdef DEBUG
  cout << "Creating list from list of " << tag << endl;
#endif
  vector<T *> *list = new vector<T *>();
  variant<monostate, IntExp *> t = monostate{};
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    if (string(tag) == "IntExp") {
      t = create_leafnode<IntExp>(ce, "IntExp", "val", ATTR_TYPE::INT);
    } else {
      cerr << "Error: Unknown element in list: " << ce->Name() << endl;
    }
    if (holds_alternative<T *>(t) == true)
      list->push_back(get<T *>(t));
    else {
      cerr << "Error: Failed to create a " << tag << endl;
      return nullptr;
    }
    ce = ce->NextSiblingElement();
  }
  return list; // empty list is also valid
}

template <class T>
T *create_leafnode(XMLElement *element, string tag, string s_attr,
                   ATTR_TYPE at) {
#ifdef DEBUG
  cout << "Creating leaf node from=" << tag << " with attrib=" << s_attr
       << endl;
#endif
  if (string(element->Name()) != tag) {
    cerr << "Error: input Element is not " << tag << endl;
    return nullptr;
  }
  const XMLAttribute *attr = element->FirstAttribute();
  variant<IntExp *, IdExp *, OpExp *> holding;
  while (attr) {
    if (string(attr->Name()) == s_attr) {
      switch (at) {
      case ATTR_TYPE::INT:
        holding =
            new IntExp(get_position(element), stoi(string(attr->Value())));
        break;
      case ATTR_TYPE::ID:
        holding = new IdExp(get_position(element), string(attr->Value()));
        break;
      case ATTR_TYPE::OP:
        holding = new OpExp(get_position(element), string(attr->Value()));
        break;
      default:
        cerr << "Error: Unknown attribute type" << endl;
        return nullptr;
      }
    }
    attr = attr->Next();
  }
  if (holds_alternative<T *>(holding) == true)
    return get<T *>(holding);
  else
    return nullptr;
}

Program *create_program(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating Program" << endl;
#endif
  MainMethod *main = nullptr;
  if (string(element->Name()) != "Program") {
    cerr << "Error: input Element is not Program" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    if (string(ce->Name()) == "MainMethod") {
      main = create_mainMethod(ce);
    } else {
      cerr << "Error: Unknown element in Program" << endl;
    }
    ce = ce->NextSiblingElement();
  }
  return new Program(get_position(element), main);
}

MainMethod *create_mainMethod(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating MainMethod" << endl;
#endif
  StmList *sl = nullptr;
  if (string(element->Name()) != "MainMethod") {
    cerr << "Error: input Element is not MainMethod" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    if (string(ce->Name()) == "StmList") {
      sl = create_stm_list(ce);
    }
    ce = ce->NextSiblingElement();
  }
  return new MainMethod(get_position(element), sl);
}

Stm *create_stm(XMLElement *element) { // stm dispatcher
#ifdef DEBUG
  cout << "Creating Stm for " << element->Name() << endl;
#endif
  if (string(element->Name()) == "Assign") {
    return create_assign(element);
  } else if (string(element->Name()) == "Return") {
    return create_return(element);
  } else {
    // cout << "Error in dispatching: Unknown element in Stm: " <<
    // element->Name()<< endl;
    return nullptr; // fail to create a statement
  }
}

StmList *create_stm_list(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating StmList, tag = " << element->Name() << endl;
#endif
  StmList *sl = new StmList();
  if (element->NoChildren()) {
    delete sl;
    sl = nullptr;
    return sl;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    Stm *s = create_stm(ce);
    if (s != nullptr)
      sl->push_back(s);
    ce = ce->NextSiblingElement();
  }
  return sl;
}

Assign *create_assign(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating Assign" << endl;
#endif
  Exp *exp1 = nullptr;
  Exp *exp2 = nullptr;
  if (string(element->Name()) != "Assign") {
    cerr << "Error: input Element is not Assign" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    Exp *e = create_exp(ce);
    if (e == nullptr)
      cerr << "Error: Assign: Unknown element in Assign" << endl;
    else {
      if (exp1 == nullptr)
        exp1 = e;
      else if (exp2 == nullptr)
        exp2 = e;
      else
        cerr << "Error: Assign: More than three expressions in Assign" << endl;
    }
    ce = ce->NextSiblingElement();
  }
  if (exp1 == nullptr || exp2 == nullptr) {
    cerr << "Error: Assign: need two expressions for assign (left, right)"
         << endl;
    return nullptr;
  }
  return new Assign(get_position(element), exp1, exp2);
}

Return *create_return(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating Return" << endl;
#endif
  Exp *exp = nullptr;
  if (string(element->Name()) != "Return") {
    cerr << "Error: input Element is not Return" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    Exp *e = create_exp(ce);
    if (e != nullptr)
      exp = e;
    else
      cerr << "Error: Return: Unknown element in Return" << endl;
    ce = ce->NextSiblingElement();
  }
  if (exp == nullptr) { // no expression,failed to create a return
    cerr << "Error: Return: No expression in Return" << endl;
    return nullptr;
  }
  return new Return(get_position(element), exp);
}

Exp *create_exp(XMLElement *element) { // exp dispatcher
#ifdef DEBUG
  cout << "Creating Exp for " << element->Name() << "..." << endl;
#endif
  if (string(element->Name()) == "BinaryOp") {
    return create_binaryOp(element);
  }
  if (string(element->Name()) == "UnaryOp") {
    return create_unaryOp(element);
  } else if (string(element->Name()) == "Esc") {
    return create_esc(element);
  } else if (string(element->Name()) == "IdExp") {
    return create_leafnode<IdExp>(element, "IdExp", "id", ATTR_TYPE::ID);
  } else if (string(element->Name()) == "IntExp") {
    return create_leafnode<IntExp>(element, "IntExp", "val", ATTR_TYPE::INT);
  } else if (string(element->Name()) == "OpExp") {
    return create_leafnode<OpExp>(element, "OpExp", "op", ATTR_TYPE::OP);
  } else {
    // cout << "Error in dispatching: Unknown element in Exp: " <<
    // element->Name()<< endl;
    return nullptr; // fail to create an expression
  }
}

BinaryOp *create_binaryOp(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating BinaryOp" << endl;
#endif
  Exp *exp1 = nullptr;
  Exp *exp2 = nullptr;
  OpExp *op = nullptr;
  if (string(element->Name()) != "BinaryOp") {
    cerr << "Error: input Element is not BinaryOp" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    if (string(ce->Name()) == "OpExp")
      op = create_leafnode<OpExp>(ce, "OpExp", "op", ATTR_TYPE::OP);
    else {
      Exp *e = create_exp(ce);
      if (exp1 == nullptr)
        exp1 = e;
      else if (exp2 == nullptr)
        exp2 = e;
    }
    ce = ce->NextSiblingElement();
  }
  if (op == nullptr) {
    cerr << "Error: BinaryOp: Missing operator" << endl;
    return nullptr;
  }
  if (exp1 == nullptr || exp2 == nullptr) {
    cerr << "Error: BinaryOp: Missing expression" << endl;
    return nullptr;
  }
  return new BinaryOp(get_position(element), exp1, op, exp2);
}

UnaryOp *create_unaryOp(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating UnaryOp" << endl;
#endif
  Exp *exp = nullptr;
  OpExp *op = nullptr;
  if (string(element->Name()) != "UnaryOp") {
    cerr << "Error: input Element is not UnaryOp" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    if (string(ce->Name()) == "OpExp")
      op = create_leafnode<OpExp>(ce, "OpExp", "op", ATTR_TYPE::OP);
    else {
      Exp *e = create_exp(ce);
      if (exp == nullptr)
        exp = e;
      else
        cerr << "Error: UnaryOp: More than one expression in UnaryOp" << endl;
    }
    ce = ce->NextSiblingElement();
  }
  return new UnaryOp(get_position(element), op, exp);
}

Esc *create_esc(XMLElement *element) {
#ifdef DEBUG
  cout << "Creating Esc" << endl;
#endif
  Exp *exp = nullptr;
  StmList *sl = nullptr;
  Stm *st = nullptr;
  if (string(element->Name()) != "Esc") {
    cerr << "Error: input Element is not Esc" << endl;
    return nullptr;
  }
  XMLElement *ce = element->FirstChildElement();
  while (ce) {
    if (string(ce->Name()) == "StmList") {
      sl = create_stm_list(ce);
    } else {
      Exp *e = create_exp(ce);
      if (e != nullptr) {
        if (exp == nullptr)
          exp = e;
        else
          cerr << "Error: Esc: More than one expression in Esc" << endl;
      }
    }
    ce = ce->NextSiblingElement();
  }
  if (exp == nullptr) {
    cerr << "Error: Esc: No expression in Esc" << endl;
    return nullptr; // fail to get an expression
  }
  if (sl != nullptr && sl->size() == 0) {
    delete sl;
    sl = nullptr;
  }
  return new Esc(get_position(element), sl, exp);
}