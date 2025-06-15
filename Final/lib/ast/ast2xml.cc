// This visitor is used to convert AST to XML
#define DEBUG
#undef DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <variant>

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "tinyxml2.hh"
#include "ast2xml.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

static bool _location_flag = true;
static bool _semant_flag = true;
static AST_Semant_Map *_semant_map = nullptr;

XMLDocument* ast2xml(Program *node, AST_Semant_Map *semant_map, bool location_flag, bool semant_flag) {
  _location_flag = location_flag;
  _semant_flag = semant_flag;
  _semant_map = semant_map;
  AST2XML v; 
  v.doc= new XMLDocument();
  XMLDeclaration *decl = v.doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
  v.doc->InsertFirstChild(decl);
#ifdef DEBUG
  std::cout << "Start Visitor: " << endl;
#endif
  node->accept(v);
  v.doc->InsertEndChild(v.el);
  return v.doc;
}

static void set_position_and_semant(XMLElement *el, const Pos *pos, AST* node) {
  #ifdef DEBUG
  cout<<"start set position and semant"<<endl;
  #endif
  if (_location_flag && pos != nullptr) {
    if (pos->sline != 0 && pos->scolumn != 0 && pos->eline != 0 && pos->ecolumn != 0) {
      el->SetAttribute("bline", to_string(pos->sline).c_str());
      el->SetAttribute("bpos", to_string(pos->scolumn).c_str());
      el->SetAttribute("eline", to_string(pos->eline).c_str());
      el->SetAttribute("epos", to_string(pos->ecolumn).c_str());
    }
  }
  if (!_semant_flag || _semant_map == nullptr) return;
  AST_Semant *semant = _semant_map->getSemant(node);
  if (semant == nullptr) {
    return;
  }
  AST_Semant::Kind kd = semant->get_kind();
  TypeKind tk = semant->get_type();
  #ifdef DEBUG
  cout<< "TypeKind: "<< static_cast<int>(tk) <<endl;
  #endif
  el->SetAttribute("s_kind", AST_Semant::s_kind_string(kd).c_str());
  if (kd == AST_Semant::Kind::Value) {
      el->SetAttribute("typeKind", fdmj::type_kind_string(tk).c_str());
      el->SetAttribute("lvalue", semant->is_lvalue() ? "true" : "false");
      switch (tk) {
        case TypeKind::CLASS:
          #ifdef DEBUG
          cout<<"TypeKind: CLASS"<< " CLASS name: "<<get<string>(semant->get_type_par())<<endl;
          #endif
          el->SetAttribute("cid", get<string>(semant->get_type_par()).c_str());
          break;
        case TypeKind::INT:
          break;
        case TypeKind::ARRAY:
          el->SetAttribute("arity", to_string(get<int>(semant->get_type_par())).c_str());
          break;
        default:
          cerr << "Error: Unknown type kind" << endl;
          break;
      }
  } else if (kd != AST_Semant::Kind::MethodName && kd != AST_Semant::Kind::ClassName) {
      Pos p(pos->sline, pos->scolumn, pos->eline, pos->ecolumn);
      cerr << "Error: at position " << p.print() << endl;
      cerr << "Error: Unknown semantic kind" << endl;
  }
  #ifdef DEBUG
  cout<<"end set position and semant"<<endl;
  #endif
}

template<class T>
XMLElement* visitList(XMLDocument* doc, AST2XML &v, vector<T*> *nl, string tag) {
#ifdef DEBUG
  cout << "visitList: tag = " << tag << endl;
  if (nl == nullptr) cout << "visitList: nl is null" << endl;
  else cout << "visitList: size = " << nl->size() << endl;
#endif
  XMLElement *cn = doc->NewElement(tag.c_str());
  if (nl == nullptr || nl->size() == 0) return cn; //empty list
  for (T* n: *nl) {
    if (n == nullptr) {
      cerr << "Error: Null element in " << tag << endl;
      continue;
    }
    n->accept(v);
    if (v.el != nullptr) cn->InsertEndChild(v.el);
  }
  return cn;
}

void AST2XML::visit(Program *node) {
    if (!node) return;
#ifdef DEBUG
    cout<<"Program"<<endl;
#endif
    XMLElement* cn = doc->NewElement("Program");
    set_position_and_semant(cn, node->getPos(), node);
    if (node->main == nullptr) {
        cerr << "Error: No MainMethod found" << endl;
        el = nullptr;
        return;
    }
    node->main->accept(*this);
    if (el != nullptr) cn->InsertEndChild(el); //insert the main method
    XMLElement *cn1 = visitList<ClassDecl>(doc, *this, node->cdl, "ClassDeclList");
    if (cn1 != nullptr) cn->InsertEndChild(cn1); //insert the class declaration
    if (cn->FirstChildElement() == nullptr) {
        cerr << "Error: No child element found in Program" << endl;
        el = nullptr;
        return;
    } 
    el = cn;
} 

void AST2XML::visit(MainMethod *node) { 
  if (!node) return;
#ifdef DEBUG
cout<<"MainMethod"<<endl;
#endif
  XMLElement *cn = doc->NewElement("MainMethod");
  set_position_and_semant(cn, node->getPos(), node);
  XMLElement *cn1 = visitList<VarDecl>(doc, *this, node->vdl, "VarDeclList");
  if (cn1 != nullptr) cn->InsertEndChild(cn1); //insert the variable declaration
  XMLElement *cn2 = visitList<Stm>(doc, *this, node->sl, "StmList");
  if (cn2 != nullptr) cn->InsertEndChild(cn2); //insert the statement
  el = cn;
}

void AST2XML::visit(ClassDecl *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"ClassDecl"<<endl;
#endif
  XMLElement *cn = doc->NewElement("ClassDecl");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->id == nullptr) {
    cerr << "Error: No class id found" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("IdExp");
  if (node->id == nullptr) {
    cerr << "Error: No id found in a ClassDecl" << endl;
    el = nullptr;
    return;
  }
  cn1->SetAttribute("id", node->id->id.c_str());
  set_position_and_semant(cn1, node->id->getPos(), node->id);
  cn->InsertEndChild(cn1);
  if (node->eid != nullptr) {
    XMLElement *cn2 = doc->NewElement("ExtendsId");
    cn2->SetAttribute("eid", node->eid->id.c_str());
    set_position_and_semant(cn2, node->eid->getPos(), node->eid);
    cn->InsertEndChild(cn2);
  }
  XMLElement *cn3 = visitList<VarDecl>(doc, *this, node->vdl, "VarDeclList");
  if (cn3 != nullptr) cn->InsertEndChild(cn3); //insert the variable declarations
  XMLElement *cn4 = visitList<MethodDecl>(doc, *this, node->mdl, "MethodDeclList");
  if (cn4 != nullptr) cn->InsertEndChild(cn4); //insert the method declarations
  el = cn;
}

void AST2XML::visit(Type *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Type" <<endl;
#endif
  XMLElement *cn = doc->NewElement("Type");
  XMLElement *cn1;
  XMLElement *cn2;
  switch (node->typeKind) {
    case TypeKind::CLASS: 
      cn->SetAttribute("typeKind", "CLASS");  
      cn1 = doc->NewElement("IdExp");
      cn1->SetAttribute("id", node->cid->id.c_str());
      set_position_and_semant(cn1, node->cid->getPos(), node->cid);
      cn->InsertEndChild(cn1);
      break;
    case TypeKind::INT:
      cn->SetAttribute("typeKind", "INT");
      break;
    case TypeKind::ARRAY:
      cn->SetAttribute("typeKind", "ARRAY");
      if (node->arity != nullptr) {
        cn2 = doc->NewElement("Arity");
        string sss = to_string(node->arity->val);
        cn2->SetAttribute("val", sss.c_str()); //to_string(node->arity->val).c_str());
        set_position_and_semant(cn2, node->arity->getPos(), node->arity);
        cn->InsertEndChild(cn2);
      }
      else {
        cerr << "Error: Array type must have an arity" << endl;
        el = nullptr;
        return;
      }
      break;
  }
  set_position_and_semant(cn, node->getPos(), node);
  el=cn;
}

void AST2XML::visit(VarDecl *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"VarDecl"<< " node id: "<< node->id->id <<endl;
#endif
  XMLElement *cn = doc->NewElement("VarDecl");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->type == nullptr) {
    cerr << "Error: No type found in a VarDecl" << endl;
    el = nullptr;
    return;
  }
  node->type->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the type
  if (node->id == nullptr) {
    cerr << "Error: No id found in a VarDecl" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("IdExp");
  cn1->SetAttribute("id", node->id->id.c_str());
  set_position_and_semant(cn1, node->id->getPos(), node->id);
  cn->InsertEndChild(cn1);
  //insert the init
  #ifdef DEBUG
  cout<<"VarDecl init"<<endl;
  #endif
  if (holds_alternative<IntExp*>(node->init) == true) {
    XMLElement *cn2 = doc->NewElement("IntInit");
    cn2->SetAttribute("val", to_string(get<IntExp*>(node->init)->val).c_str());
    set_position_and_semant(cn2, get<IntExp*>(node->init)->getPos(), get<IntExp*>(node->init));
    cn->InsertEndChild(cn2);
  } else if (holds_alternative<vector<IntExp*>*>(node->init) == true) {
    vector<IntExp*> *v = get<vector<IntExp*>*>(node->init);
    if (v != nullptr) {
      XMLElement *cn3 = doc->NewElement("IntInitList");
      for (IntExp *i : *v) {
        XMLElement *cn4 = doc->NewElement("IntExp");
        cn4->SetAttribute("val", to_string(i->val).c_str());
        set_position_and_semant(cn4, i->getPos(), i);
        cn3->InsertEndChild(cn4);
      } 
      cn->InsertEndChild(cn3);
    }
  }
  #ifdef DEBUG
  cout<<"VarDecl done"<<endl;
  #endif
  el = cn;
}

void AST2XML::visit(MethodDecl *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"MethodDecl"<<endl;
#endif
  XMLElement *cn = doc->NewElement("MethodDecl");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->type == nullptr) {
    cerr << "Error: No return type found in a MethodDecl" << endl;
    el = nullptr;
    return;
  }
  node->type->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the type
  if (node->id == nullptr) {
    cerr << "Error: No method id found in a MethodDecl" << endl;
    el = nullptr;
    return;
  }
  // insert the method id
  XMLElement *cn1 = doc->NewElement("IdExp");
  cn1->SetAttribute("id", node->id->id.c_str());
  set_position_and_semant(cn1, node->id->getPos(), node->id);
  cn->InsertEndChild(cn1);
  XMLElement *cn2 = visitList<Formal>(doc, *this, node->fl, "FormalList");
  if (cn2 != nullptr) cn->InsertEndChild(cn2); //insert the formal
  XMLElement *cn3 = visitList<VarDecl>(doc, *this, node->vdl, "VarDeclList");
  if (cn3 != nullptr) cn->InsertEndChild(cn3); //insert the variable declaration
  XMLElement *cn4 = visitList<Stm>(doc, *this, node->sl, "StmList");
  if (cn4 != nullptr) cn->InsertEndChild(cn4); //insert the statement
  el = cn;
}

void AST2XML::visit(Formal *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Formal"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Formal");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->type == nullptr) {
    cerr << "Error: No type found in a Formal" << endl;
    el = nullptr;
    return;
  }
  node->type->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the type
  if (node->id == nullptr) {
    cerr << "Error: No id found in a Formal" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("IdExp");
  set_position_and_semant(cn1, node->id->getPos(), node->id);
  cn1->SetAttribute("id", node->id->id.c_str());
  cn->InsertEndChild(cn1);
  el = cn;
}

void AST2XML::visit(Nested *node) {
  if (!node) return;
#ifdef DEBUG
cout<< "Nested" <<endl;
#endif
  XMLElement *cn = doc->NewElement("Nested");
  set_position_and_semant(cn, node->getPos(), node);
  XMLElement *cn1 = visitList<Stm>(doc, *this, node->sl, "StmList");
  if (cn1 != nullptr) cn->InsertEndChild(cn1);
  el = cn;
}

void AST2XML::visit(If *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"If"<<endl;
#endif
  XMLElement *cn = doc->NewElement("If");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in an If" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  if (node->stm1 == nullptr) {
    cerr << "Error: No statement found in an If" << endl;
    el = nullptr;
    return;
  }
  node->stm1->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the statement
  if (node->stm2 != nullptr) {
    node->stm2->accept(*this);
    if (el != nullptr) cn->InsertEndChild(el); //insert the statement
  }
  el = cn;
}

void AST2XML::visit(While *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"While"<<endl;
#endif
  XMLElement *cn = doc->NewElement("While");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a While" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  if (node->stm != nullptr) {
    node->stm->accept(*this);
    if (el != nullptr) cn->InsertEndChild(el); //insert the statement
  }
  el = cn;
}

void AST2XML::visit(Assign *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Assign"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Assign");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->left == nullptr) {
    cerr << "Error: No left expression found in an Assign" << endl;
    el = nullptr;
    return;
  }
  node->left->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the left expression
  if (node->exp == nullptr) {
    cerr << "Error: No right expression found in an Assign" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the right expression
  el = cn;
}

void AST2XML::visit(CallStm *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"CallStm"<<endl;
#endif
  XMLElement *cn = doc->NewElement("CallStm");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->obj == nullptr) {
    cerr << "Error: No object found in a CallStm" << endl;
    el = nullptr;
    return;
  }
  node->obj->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the object
  if (node->name== nullptr) {
    cerr << "Error: No method id found in a CallStm" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("IdExp");
  cn1->SetAttribute("id", node->name->id.c_str());
  set_position_and_semant(cn1, node->name->getPos(), node->name);
  cn->InsertEndChild(cn1);
  XMLElement *cn2 = visitList<Exp>(doc, *this, node->par, "ParList");
  if (cn2 != nullptr) cn->InsertEndChild(cn2); //insert the parameters
  el = cn;
}

void AST2XML::visit(Continue *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Continue"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Continue");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(Break *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Break"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Break");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(Return *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Return"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Return");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  }
  el = cn;
}

void AST2XML::visit(PutInt *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"PutInt"<<endl;
#endif
  XMLElement *cn = doc->NewElement("PutInt");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a PutInt" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  el = cn;
}

void AST2XML::visit(PutCh *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"PutCh"<<endl;
#endif
  XMLElement *cn = doc->NewElement("PutCh");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a PutCh" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  el = cn;
}

void AST2XML::visit(PutArray *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"PutArray"<<endl;
#endif
  XMLElement *cn = doc->NewElement("PutArray");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->arr == nullptr) {
    cerr << "Error: No array expression found in a PutArray" << endl;
    el = nullptr;
    return;
  }
  node->n->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the index expression
  node->arr->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the array expression
  if (node->n == nullptr || node->arr == nullptr) {
    cerr << "Error: No array or length expression found in a PutArray" << endl;
    el = nullptr;
    return;
  }
  el = cn;
}

void AST2XML::visit(Starttime *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Starttime"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Starttime");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(Stoptime *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Stoptime"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Stoptime");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(BinaryOp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"BinaryOp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("BinaryOp");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->left == nullptr) {
    cerr << "Error: No left expression found in a BinaryOp" << endl;
    el = nullptr;
    return;
  }
  node->left->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the left expression
  if (node->op == nullptr) {
    cerr << "Error: No operator found in a BinaryOp" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("OpExp");
  cn1->SetAttribute("op", node->op->op.c_str());
  set_position_and_semant(cn1, node->op->getPos(), node->op);
  cn->InsertEndChild(cn1);
  if (node->right == nullptr) {
    cerr << "Error: No right expression found in a BinaryOp" << endl;
    el = nullptr;
    return;
  }
  node->right->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the right expression
  el = cn;
}

void AST2XML::visit(UnaryOp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"UnaryOp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("UnaryOp");
  set_position_and_semant(cn, node->getPos(), node);
  XMLElement *cn1 = doc->NewElement("OpExp");
  cn1->SetAttribute("op", node->op->op.c_str());
  set_position_and_semant(cn1, node->op->getPos(), node->op);
  cn->InsertEndChild(cn1);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a UnaryOp" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  el = cn;
}

void AST2XML::visit(ArrayExp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"ArrayExp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("ArrayExp");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->arr == nullptr) {
    cerr << "Error: No array expression found in an ArrayExp" << endl;
    el = nullptr;
    return;
  }
  node->arr->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the array expression
  if (node->index == nullptr) {
    cerr << "Error: No index expression found in an ArrayExp" << endl;
    el = nullptr;
    return;
  }
  node->index->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the index expression
  el = cn;
}

void AST2XML::visit(CallExp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"CallExp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("CallExp");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->obj == nullptr) {
    cerr << "Error: No object found in a CallExp" << endl;
    el = nullptr;
    return;
  }
  node->obj->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the object
  if (node->name == nullptr) {
    cerr << "Error: No method id found in a CallExp" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("IdExp");
  cn1->SetAttribute("id", node->name->id.c_str());
  set_position_and_semant(cn1, node->name->getPos(), node->name);
  cn->InsertEndChild(cn1);
  XMLElement *cn2 = visitList<Exp>(doc, *this, node->par, "ParList");
  if (cn2 != nullptr) cn->InsertEndChild(cn2); //insert the parameters
  el = cn;
}

void AST2XML::visit(ClassVar *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"ClassVar"<<endl;
#endif
  XMLElement *cn = doc->NewElement("ClassVar");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->obj == nullptr) {
    cerr << "Error: No object found in a ClassVar" << endl;
    el = nullptr;
    return;
  }
  node->obj->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the object
  if (node->id == nullptr) {
    cerr << "Error: No id found in a ClassVar" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("IdExp");
  cn1->SetAttribute("id", node->id->id.c_str());
  set_position_and_semant(cn1, node->id->getPos(), node->id);
  cn->InsertEndChild(cn1);
  el = cn;
}

void AST2XML::visit(BoolExp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"BoolExp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("BoolExp");
  set_position_and_semant(cn, node->getPos(), node);
  cn->SetAttribute("val", to_string(node->val).c_str());
  el = cn;
}

void AST2XML::visit(This *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"This"<<endl;
#endif
  XMLElement *cn = doc->NewElement("This");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(Length *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Length"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Length");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a Length" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  el = cn;
}

void AST2XML::visit(GetInt *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"GetInt"<<endl;
#endif
  XMLElement *cn = doc->NewElement("GetInt");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(GetCh *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"GetCh"<<endl;
#endif
  XMLElement *cn = doc->NewElement("GetCh");
  set_position_and_semant(cn, node->getPos(), node);
  el = cn;
}

void AST2XML::visit(GetArray *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"GetArray"<<endl;
#endif
  XMLElement *cn = doc->NewElement("GetArray");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a GetArray" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  el = cn;
}

void AST2XML::visit(Esc *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"Esc"<<endl;
#endif
  XMLElement *cn = doc->NewElement("Esc");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->sl != nullptr && node->sl->size() > 0) {
    XMLElement* cn1 = visitList<Stm>(doc, *this, node->sl, "StmList");
    if (cn1 != nullptr) cn->InsertEndChild(cn1); //insert the statement list
  }
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    if (el != nullptr) cn->InsertEndChild(el); //insert the expression
  }
  el = cn;
}

void AST2XML::visit(IdExp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"IdExp: "<< node->id <<endl;
#endif
  XMLElement *cn = doc->NewElement("IdExp");
  set_position_and_semant(cn, node->getPos(), node);
  if (node->id.empty()) {
    cerr << "Error: No id found in an IdExp" << endl;
    el = nullptr;
    return;
  }
  cn->SetAttribute("id", node->id.c_str());
  set_position_and_semant(cn, node->getPos(), nullptr);
  el = cn;
}

void AST2XML::visit(IntExp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"IntExp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("IntExp");
  set_position_and_semant(cn, node->getPos(), node);
  cn->SetAttribute("val", to_string(node->val).c_str());
  el = cn;
}

void AST2XML::visit(OpExp *node) {
  if (!node) return;
#ifdef DEBUG
cout<<"OpExp"<<endl;
#endif
  XMLElement *cn = doc->NewElement("OpExp");
  set_position_and_semant(cn, node->getPos(), nullptr);
  cn->SetAttribute("op", node->op.c_str());
  el = cn;
}