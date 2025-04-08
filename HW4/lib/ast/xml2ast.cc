// This file is used to convert AST to XML

#define DEBUG
#undef DEBUG

#include <iostream>
#include <algorithm>
#include <variant>
#include <cctype>
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "namemaps.hh"
#include "semant.hh"
#include "xml2ast.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

#define ClassDeclList vector<ClassDecl*>
#define VarDeclList vector<VarDecl*>
#define MethodDeclList vector<MethodDecl*>
#define FormalList vector<Formal*>
#define StmList vector<Stm*>
#define ExpList vector<Exp*>

Name_Maps* create_NameMaps(XMLElement* xml_root); //forward declaration
static AST_Semant_Map *global_semant_map = new AST_Semant_Map();

Program* xml2ast(XMLElement *element, AST_Semant_Map **sm) {
    if (string(element->Name()) != "FDMJAST") {
        cerr << "Error: first element is not FDMJAST" << endl;
        return nullptr;
    }
    XMLElement *nm_root = element->FirstChildElement();
    XMLElement *program;
    if (string(nm_root->Name()) == "NameMaps") {
        global_semant_map->setNameMaps(create_NameMaps(nm_root));
        program = nm_root->NextSiblingElement(); 
    } else {
        XMLElement *program = element->FirstChildElement();
    }
    if (string(program->Name()) != "Program") {
        cerr << "Error: Root element is not Program" << endl;
        return nullptr;
    }
    Program* prog = create_program(program);
    if (prog == nullptr) {
        cerr << "Error: Failed to create Program" << endl;
        return nullptr;
    }
    *sm = global_semant_map;
#ifdef DEBUG
    if ((*sm)->getNameMaps() == nullptr) {
        cerr << "----Error: No NameMaps found" << endl;
        return nullptr;
    } else {
        (*sm)->getNameMaps()->print();
    }
#endif
    return prog;
}

Program* xml2ast(string xmlfilename, AST_Semant_Map **sm) {
    XMLDocument doc;
    if (doc.LoadFile(xmlfilename.c_str()) != XML_SUCCESS) {
        cerr << "Error: " << xmlfilename << " not found" << endl;
        return nullptr;
    }
    if (doc.ErrorID() != XML_SUCCESS) {
        cerr << "Error: " << xmlfilename << " is not a valid XML file" << endl;
        return nullptr;
    }
    return xml2ast(doc.RootElement(), sm);
}

static Pos *get_position(XMLElement *element) {
#ifdef DEBUG
    //cout << "Getting position" << endl;
#endif
    int bline = 0, bpos = 0, eline = 0, epos = 0;
    const XMLAttribute *attr = element->FirstAttribute();
    while (attr) { 
        string name = attr->Name();
        if (name == "bline") { bline = stoi(string(attr->Value()));
        } else if (name == "bpos") { bpos = stoi(string(attr->Value()));
        } else if (name == "eline") { eline = stoi(string(attr->Value()));
        } else if (name == "epos") { epos = stoi(string(attr->Value()));}
        attr = attr->Next();
    }
    //the XML file may not have positions info in it 
    /*if (bline == 0 || bpos == 0 || eline == 0 || epos == 0) {
        //cerr << "Error: No position found in " << string(element->Name()) << ", or position is ill formed" << endl;
        return nullptr;
    */
    return new Pos(bline, bpos, eline, epos);
}

static AST_Semant *get_semant(XMLElement *element) {
#ifdef DEBUG
    cout << "Getting semantic information from node " << element->Name() << endl;
#endif
    AST_Semant::Kind s_kind;
    TypeKind typeKind = TypeKind::INT;
    variant<monostate, string, int> type_par = monostate{};
    bool lvalue = false;
    bool has_semant_info = false; //go through the attributes to see if there is semantic info
    const XMLAttribute *attr = element->FirstAttribute();
    while (attr) {
        string name = string(attr->Name());
        string value = string(attr->Value());
        //cout << "-----name=" << name << " value = " << value << endl;
        if (name == "s_kind") {
            if (value == "Value") { s_kind = AST_Semant::Kind::Value;
            } else if (value == "MethodName") { s_kind = AST_Semant::Kind::MethodName;
            } else if (value == "ClassName") { s_kind = AST_Semant::Kind::ClassName;
            } else {
                cerr << "Error: Unknown semantic kind" << endl;
                return nullptr;
            }
            has_semant_info = true; //if we have s_kind, we should have the rest
        } else if (name == "typeKind") {
            if (value == "INT") { typeKind = TypeKind::INT;
            } else if (value == "CLASS") { typeKind = TypeKind::CLASS;
            } else if (value == "INTARRAY") { typeKind = TypeKind::ARRAY;
            } else {
                cerr << "Error: Unknown type kind: " << value << endl;
                return nullptr;
            }
        } else if (name == "type_par") {
            string type_par_str = string(attr->Value());
            if (typeKind == TypeKind::ARRAY) {
                type_par = stoi(type_par_str);
            } else if (typeKind == TypeKind::CLASS) {
                type_par = type_par_str;
            } else {
                cerr << "Error: Unknown type kind for type parameter" << endl;
                return nullptr;
            }
        } else if (name == "lvalue") {
            lvalue = string(attr->Value()) == "true"?true:false;
        }
        attr = attr->Next();
    }
    if (has_semant_info) 
        return new AST_Semant(s_kind, typeKind, type_par, lvalue);
    else return nullptr;
}

/*
template<class T>
T* create_(XMLElement *element, string tag) {
    //dispatch to the right create function
    if (string(element->Name()) != tag) {
        cerr << "Error: input Element is not " << tag << endl;
        return nullptr;
    }
    T* what; // for dispactching to the right create function
    return static_cast<T*>(create_astnode(what, element));
}
*/

//forward declarations
Program* create_program(XMLElement*);
MainMethod* create_mainMethod(XMLElement*);
ClassDecl* create_classDecl(XMLElement*);
Type* create_type(XMLElement*);
VarDecl* create_varDecl(XMLElement*);
MethodDecl* create_methodDecl(XMLElement*);
Formal* create_formal(XMLElement*);
Nested* create_nested(XMLElement*);
If* create_if(XMLElement*);
While* create_while(XMLElement*);
Assign* create_assign(XMLElement*);
CallStm* create_callStm(XMLElement*);
Continue* create_continue(XMLElement*);
Break* create_break(XMLElement*);
Return* create_return(XMLElement*);
PutInt* create_putInt(XMLElement*);
PutCh* create_putCh(XMLElement*);
PutArray* create_putArray(XMLElement*);
Starttime* create_starttime(XMLElement*);
Stoptime* create_stoptime(XMLElement*);
BinaryOp* create_binaryOp(XMLElement*);
UnaryOp* create_unaryOp(XMLElement*);
ArrayExp* create_arrayExp(XMLElement*);
CallExp* create_callExp(XMLElement*);
ClassVar* create_classVar(XMLElement*);
BoolExp* create_boolExp(XMLElement*);
This* create_this(XMLElement*);
Length* create_length(XMLElement*);
Esc* create_esc(XMLElement*);
GetInt* create_getInt(XMLElement*);
GetCh* create_getCh(XMLElement*);
GetArray* create_getArray(XMLElement*);
//IdExp* create_idExp(XMLElement*);
//IntExp* create_intExp(XMLElement*);
//OpExp* create_opExp(XMLElement*);

ExpList* create_exp_list(XMLElement*);
StmList* create_stm_list(XMLElement*);

enum class ATTR_TYPE {
    INT,
    ID,
    OP,
    BOOL 
};
template<class T> T* create_leafnode(XMLElement*, string, string, ATTR_TYPE); //forward declaration

template<class T> //only works for lists of VarDecl, MethodDecl, Formal, ClassDecl, IntExp
vector<T*> *create_list(XMLElement *element, string tag) {
#ifdef DEBUG
    cout << "Creating list from list of " << tag << endl;
#endif
    vector<T*> *list = new vector<T*>();
    variant<monostate, VarDecl*, MethodDecl*, Formal*, ClassDecl*, IntExp*> t = monostate{};
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        AST_Semant *se = get_semant(ce); //read the semant info of the node
        if (string(tag) == "VarDecl") {
            t = create_varDecl(ce);
        } else if (string(tag) == "MethodDecl") {
            t = create_methodDecl(ce);
        } else if (string(tag) == "Formal") {
            t = create_formal(ce);
        } else if (string(tag) == "ClassDecl") {
            t = create_classDecl(ce);
        } else if (string(tag) == "IntExp") {
            t = create_leafnode<IntExp>(ce, "IntExp", "val", ATTR_TYPE::INT);
        } else {
            cerr << "Error: Unknown element in list: " << ce->Name() << endl;
            return nullptr;
        }
        if (holds_alternative<T*>(t) == true) {
            if (se != nullptr) global_semant_map->setSemant(get<T*>(t), se); //add semant to map
            list->push_back(get<T*>(t));
        }
        else {
            cerr << "Error: Failed to create a " << tag << endl;
            return nullptr;
        }
        ce = ce->NextSiblingElement();
    }
    return list; //empty list is also valid 
}

template<class T>
T* create_leafnode(XMLElement *element, string tag, string s_attr, ATTR_TYPE at) {
#ifdef DEBUG
    cout << "Creating leaf node from=" << tag << " with attrib=" << s_attr << endl;
#endif
    if (string(element->Name()) != tag) {
        cerr << "Error: input Element is not " << tag << endl;
        return nullptr;
    }
    AST_Semant* se = get_semant(element); //read the semant info of the node
    const XMLAttribute *attr = element->FirstAttribute();
    variant<IntExp*, IdExp*, OpExp*, BoolExp*> holding;
    while (attr) {
        if (string(attr->Name()) == s_attr) {
            switch (at) {
                case ATTR_TYPE::INT:
                    holding = new IntExp(get_position(element), stoi(string(attr->Value())));
                    break;
                case ATTR_TYPE::ID:
                    holding = new IdExp(get_position(element), string(attr->Value()));
                    break;
                case ATTR_TYPE::OP:
                    holding = new OpExp(get_position(element), string(attr->Value()));
                    break;
                case ATTR_TYPE::BOOL:
                    holding = new BoolExp(get_position(element), string(attr->Value())=="1"?true:false);
                    break;
                default:
                    cerr << "Error: Unknown attribute type" << endl;
                    return nullptr;
            }
        }
        attr = attr->Next();
    }
    if (holds_alternative<T*>(holding) == true)  {
        if (se != nullptr) global_semant_map->setSemant(get<T*>(holding), se); //add semant to map
        return get<T*>(holding);
    }
    else return nullptr;
}

Program* create_program(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Program" << endl;
#endif
    MainMethod *main = nullptr;
    ClassDeclList *cdl = new ClassDeclList();
    if (string(element->Name()) != "Program") {
        cerr << "Error: input Element is not Program" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "MainMethod" ) {
            main = create_mainMethod(ce);
        }
        else  if (string(ce->Name()) == "ClassDeclList" ) {
            if (ce->NoChildren()) {
                delete cdl; cdl = nullptr;
            } else {
                cdl = create_list<ClassDecl>(ce, "ClassDecl");
            }
        } else {
            cerr << "Error: Unknown element in Program" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    return new Program(get_position(element), main, cdl);
}   

MainMethod* create_mainMethod(XMLElement* element) {
#ifdef DEBUG
cout << "Creating MainMethod" << endl;
#endif
    VarDeclList *vdl = nullptr;
    StmList *sl = nullptr;
    if (string(element->Name()) != "MainMethod") {
        cerr << "Error: input Element is not MainMethod" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "VarDeclList" ) {
            vdl = create_list<VarDecl>(ce, "VarDecl");
        } else if (string(ce->Name()) == "StmList" ) {
            sl = create_stm_list(ce);
        }
        ce = ce->NextSiblingElement();
    } 
    return new MainMethod(get_position(element), vdl, sl);
}

ClassDecl* create_classDecl(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating ClassDecl" << endl;
#endif
    IdExp *id = nullptr;
    IdExp *eid = nullptr;
    VarDeclList *vdl = nullptr;
    MethodDeclList *mdl = nullptr;
    if (string(element->Name()) != "ClassDecl") {
        cerr << "Error: input Element is not ClassDecl" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "IdExp" ) {
            id = create_leafnode<IdExp>(ce, "IdExp", "id", ATTR_TYPE::ID);
        }
        else if (string(ce->Name()) == "ExtendsId") {
            eid = create_leafnode<IdExp>(ce, "ExtendsId", "eid", ATTR_TYPE::ID);
        }
        else  if (string(ce->Name()) == "VarDeclList" ) {
            vdl = create_list<VarDecl>(ce, "VarDecl");
        }
        else  if (string(ce->Name()) == "MethodDeclList" ) {
            mdl = create_list<MethodDecl>(ce, "MethodDecl");
        }
        else {
            cerr << "Error: Unknown element in ClassDecl: " << ce->Name() << endl;
        }
        ce = ce->NextSiblingElement();
    }   
    if (id == nullptr) {
        cerr << "Error: ClassDecl: Class id can't be null" << endl;
        return nullptr;
    }
    return new ClassDecl(get_position(element), id, eid, vdl, mdl);
}

Type* create_type(XMLElement* element) {
    TypeKind typeKind;
    IdExp *cid = nullptr;
    IntExp *arity = nullptr;
    if (string(element->Name()) != "Type") {
        cerr << "Error: input Element is not Type" << endl;
        return nullptr;
    }
    //get the typeKind
    IdExp *tk = create_leafnode<IdExp>(element, "Type", "typeKind", ATTR_TYPE::ID);
    if (tk->id == "CLASS") typeKind = TypeKind::CLASS;
    else if (tk->id == "INT") typeKind = TypeKind::INT;
    else if (tk->id == "INTARRAY") typeKind = TypeKind::ARRAY;
    else {
        cerr << "Error: Unknown TypeKind!" << endl;
        return nullptr;
    }
    XMLElement *ce, *e1;
    Pos *p1;
    switch (typeKind) { //get the arity
    case TypeKind::ARRAY:
        ce = element->FirstChildElement();
        while (ce) {
            if (string(ce->Name()) == "Arity") {
                arity = create_leafnode<IntExp>(ce, "Arity", "val", ATTR_TYPE::INT);
            }
            ce = ce->NextSiblingElement();
        }
        if (arity == nullptr) {
            cerr << "Error: Array type must have an arity (can be 0)" << endl;
            return nullptr;
        }
        break;
    case TypeKind::CLASS:
        e1 = element->FirstChildElement();
        p1 = get_position(e1);
        IdExp *t_cid;
        while (e1) {
            if (string(e1->Name()) == "IdExp") {
                t_cid = create_leafnode<IdExp>(e1, "IdExp", "id", ATTR_TYPE::ID);
            } 
            e1 = e1->NextSiblingElement();
        }
        if (t_cid == nullptr) {
            cerr << "Error: Class type must have a class id" << endl;
            return nullptr;
        } else 
            cid = t_cid;
        break;
    }
    return new Type(get_position(element), typeKind, cid, arity);
}

VarDecl* create_varDecl(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating VarDecl" << endl;
#endif
    Type *type = nullptr;
    IdExp *id = nullptr;
    variant<monostate, IntExp *, vector<IntExp*>*> init = monostate{};
    if (string(element->Name()) != "VarDecl") {
        cerr << "Error: input Element is not VarDecl" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "Type" ) {
            type = create_type(ce);
        }
        else  if (string(ce->Name()) == "IdExp" ) {
            id = create_leafnode<IdExp>(ce, "IdExp", "id", ATTR_TYPE::ID);
        }
        ce = ce->NextSiblingElement();
    }
    if (type == nullptr || id == nullptr) {
        cerr << "Error: VarDecl: Type or Id can't be null" << endl;
        return nullptr;
    }
    XMLElement *ce1 = element->FirstChildElement();
    switch (type->typeKind) {
        case TypeKind::INT: //look for int init
            while (ce1) {
                if (string(ce1->Name()) == "IntInit") {
                    IntExp *ii = create_leafnode<IntExp>(ce1, "IntInit", "val", ATTR_TYPE::INT);
                    init = ii;
                }
                ce1 = ce1->NextSiblingElement();
            }
            break;
        case TypeKind::ARRAY: //look for array init  
            vector <IntExp*> *init_array = nullptr;
            while (ce1) {
                if (string(ce1->Name()) == "IntInitList") {
                    init_array = create_list<IntExp>(ce1, "IntExp");
                    if (init_array == nullptr) init_array = new vector<IntExp*>();
                    //else init = init_array;
                }
                ce1 = ce1->NextSiblingElement();
            }
            init = init_array;
            break;
    }
    return new VarDecl(get_position(element), type, id, init);
}

MethodDecl* create_methodDecl(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating MethodDecl" << endl;
#endif
    Type *type = nullptr;
    IdExp *id = nullptr;
    FormalList *fl = new FormalList();
    VarDeclList *vdl = new VarDeclList();
    StmList *sl = new StmList();
    if (string(element->Name()) != "MethodDecl") {
        cerr << "Error: input Element is not MethodDecl" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "Type" ) {
            type = create_type(ce);
        }
        else if (string(ce->Name()) == "IdExp" ) {
            id = create_leafnode<IdExp>(ce, "IdExp", "id", ATTR_TYPE::ID);
        }
        else if (string(ce->Name()) == "FormalList" ) {
            fl = create_list<Formal>(ce, "Formal");
        }
        else  if (string(ce->Name()) == "VarDeclList" ) {
            vdl = create_list<VarDecl>(ce, "VarDecl");
        }
        else {
            sl = create_stm_list(ce);
        }
        ce = ce->NextSiblingElement();
    }
    if (type == nullptr || id == nullptr) {
        cerr << "Error: MethodDecl: Type or Id can't be null" << endl;
        return nullptr;
    }
    return new MethodDecl(get_position(element), type, id, fl, vdl, sl);
}

Formal* create_formal(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Formal" << endl;
#endif
    Type *type = nullptr;
    IdExp *id = nullptr;
    if (string(element->Name()) != "Formal") {
        cerr << "Error: input Element is not Formal" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "Type" ) {
            type = create_type(ce);
        }
        else  if (string(ce->Name()) == "IdExp" ) {
            id = create_leafnode<IdExp>(ce, "IdExp", "id", ATTR_TYPE::ID);
        }
        else {
            cerr << "Error: Unknown element in Formal" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    return new Formal(get_position(element), type, id);
}

Stm* create_stm(XMLElement* element) { //stm dispatcher
#ifdef DEBUG
    cout << "Creating Stm for " << element->Name() << endl;
#endif
    if (string(element->Name()) == "Nested") {
        return create_nested(element);
    }
    else if (string(element->Name()) == "If") {
        return create_if(element);
    }
    else if (string(element->Name()) == "While") {
        return create_while(element);
    }
    else if (string(element->Name()) == "Assign") {
        return create_assign(element);
    }
    else if (string(element->Name()) == "CallStm") {
        return create_callStm(element);
    }
    else if (string(element->Name()) == "Continue") {
        return create_continue(element);
    }
    else if (string(element->Name()) == "Break") {
        return create_break(element);
    }
    else if (string(element->Name()) == "Return") {
        return create_return(element);
    }
    else if (string(element->Name()) == "PutInt") {
        return create_putInt(element);
    }
    else if (string(element->Name()) == "PutCh") {
        return create_putCh(element);
    }
    else if (string(element->Name()) == "PutArray") {
        return create_putArray(element);
    }
    else if (string(element->Name()) == "Starttime") {
        return create_starttime(element);
    }
    else if (string(element->Name()) == "Stoptime") {
        return create_stoptime(element);
    }
    else {
        //cout << "Error in dispatching: Unknown element in Stm: " << element->Name()<< endl;
        return nullptr; //fail to create a statement
    }
}

StmList* create_stm_list(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating StmList, tag = " << element->Name() << endl;
#endif
    StmList *sl = new StmList();
    if (element->NoChildren()) {
        //delete sl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Stm *s = create_stm(ce);
        if (s != nullptr) sl->push_back(s);
        ce = ce->NextSiblingElement();
    }
    return sl;
}

Nested* create_nested(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Nested" << endl;
#endif
    if (string(element->Name()) != "Nested") {
        cerr << "Error: input Element is not Nested" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    if (string(ce->Name()) != "StmList") {
        cerr << "Error: Nested: No StmList found in Nested" << endl;
        return nullptr;
    }
    return new Nested(get_position(element), create_stm_list(ce));
}

If* create_if(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating If" << endl;
#endif
    Exp *exp = nullptr;
    Stm *stm1 = nullptr;
    Stm *stm2 = nullptr;
    if (string(element->Name()) != "If") {
        cerr << "Error: input Element is not If" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Stm *s = create_stm(ce);
        if (s != nullptr) {
           if (stm1 == nullptr) stm1=s;
           else if (stm2 == nullptr) stm2=s;
           else cerr << "Error: If: More than two statements in If" << endl;
        } else {
            Exp *e = create_exp(ce);
            if (e != nullptr) exp = e;
            else cerr << "Error: If: Unknown element in If" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    return new If(get_position(element), exp, stm1, stm2);
}

While* create_while(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating While" << endl;
#endif
    Exp *exp = nullptr;
    Stm *stm = nullptr;
    if (string(element->Name()) != "While") {
        cerr << "Error: input Element is not While" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Stm *s = create_stm(ce);
        if (s != nullptr) {
            if (stm == nullptr) stm=s;
            else cerr << "Error: While: More than one statement in While" << endl;
        } else {
            Exp *e = create_exp(ce);
            if (e != nullptr) exp = e;
            else cerr << "Error: While: Unknown element in While" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    return new While(get_position(element), exp, stm);
}

Assign* create_assign(XMLElement* element) {
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
            if (exp1 == nullptr) exp1 = e;
            else if (exp2 == nullptr) exp2 = e; 
            else cerr << "Error: Assign: More than three expressions in Assign" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    if (exp1 == nullptr || exp2 == nullptr) {
        cerr << "Error: Assign: need two expressions for assign (left, right)" << endl;
        return nullptr;
    }
    return new Assign(get_position(element), exp1, exp2);
}

CallStm* create_callStm(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating CallStm" << endl;
#endif
    Exp *exp = nullptr;
    IdExp *id = nullptr;
    ExpList *par = new ExpList();
    if (string(element->Name()) != "CallStm") {
        cerr << "Error: input Element is not CallStm" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "ParList")
            par = create_exp_list(ce);
        else  { //The other elements are expressions, id (and in this order)
            Exp *e = create_exp(ce);
            if (exp == nullptr) //the first express must be the object expression (if it happens to be and ID, it's ok too)
                exp = e;
            else if (id == nullptr && e->getASTKind()==ASTKind::IdExp) 
                id = static_cast<IdExp*>(e);
            else {
                cerr << "Error: CallStm: More than one expression in CallStm" << endl;
                return nullptr;
            }
        }
        ce = ce->NextSiblingElement();
    }
    if (id == nullptr) {
        cerr << "Error: CallStm: method id can't be null" << endl;
        return nullptr;
    }
    if (exp == nullptr) {
        cerr << "Error: CallStm: object expression can't be null" << endl;
        return nullptr;
    }
    return new CallStm(get_position(element), exp, id, par);
}

Continue* create_continue(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Continue" << endl;
#endif
    if (string(element->Name()) != "Continue") {
        cerr << "Error: input Element is not Continue" << endl;
        return nullptr;
    }
    return new Continue(get_position(element));
}

Break* create_break(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Break" << endl;
#endif
    if (string(element->Name()) != "Break") {
        cerr << "Error: input Element is not Break" << endl;
        return nullptr;
    }
    return new Break(get_position(element));
}

Return* create_return(XMLElement* element) {
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
        if (e != nullptr) exp = e;
        else cerr << "Error: Return: Unknown element in Return" << endl;
        ce = ce->NextSiblingElement();
    }
    if (exp == nullptr) { //no expression,failed to create a return
        cerr << "Error: Return: No expression in Return" << endl;
        return nullptr;
    }
    return new Return(get_position(element), exp);
}

PutInt* create_putInt(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating PutInt" << endl;
#endif
    Exp *exp = nullptr;
    if (string(element->Name()) != "PutInt") {
        cerr << "Error: input Element is not PutInt" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (e != nullptr) exp = e;
        else cerr << "Error: PutInt: unknown lement in PutInt" << endl;
        ce = ce->NextSiblingElement();
    }
    if (exp == nullptr) { //no expression,failed to create a PutInt
        return nullptr;
    }
    return new PutInt(get_position(element), exp);
}

PutCh* create_putCh(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating PutCh" << endl;
#endif
    Exp *exp = nullptr;
    if (string(element->Name()) != "PutCh") {
        cerr << "Error: input Element is not PutCh" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (exp == nullptr) exp = e;
            else cerr << "Error: PutCh: More than one expression in PutCh" << endl;
        ce = ce->NextSiblingElement();
    }
    return new PutCh(get_position(element), exp);
}

PutArray* create_putArray(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating PutArray" << endl;
#endif
    Exp *exp1 = nullptr;
    Exp *exp2 = nullptr;
    if (string(element->Name()) != "PutArray") {
        cerr << "Error: input Element is not PutArray" << endl;
        return nullptr;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (exp1 == nullptr) exp1 = e; 
        else if (exp2 == nullptr) exp2 = e;
        else cerr << "Error: PutArray: More than two expressions in PutArray" << endl;
        ce = ce->NextSiblingElement();
    }
    return new PutArray(get_position(element), exp1, exp2);
}

Starttime* create_starttime(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Starttime" << endl;
#endif
    if (string(element->Name()) != "Starttime") {
        cerr << "Error: input Element is not Starttime" << endl;
        return nullptr;
    }
    return new Starttime(get_position(element));
}

Stoptime* create_stoptime(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Stoptime" << endl;
#endif
    if (string(element->Name()) != "Stoptime") {
        cerr << "Error: input Element is not Stoptime" << endl;
        return nullptr;
    }
    return new Stoptime(get_position(element));
}

ExpList* create_exp_list(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating ExpList for " << element->Name() << endl;
#endif
    ExpList *el = new ExpList();
    if (element->NoChildren()) {
        delete el; el = nullptr;
        return el;
    }
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp* e = static_cast<Exp*>(create_exp(ce));
        if (e != nullptr) el->push_back(e);
        ce = ce->NextSiblingElement();
    }
    return el;
}

Exp* create_exp(XMLElement* element) { //exp dispatcher
#ifdef DEBUG
    cout << "Creating Exp for " << element->Name() << "..." << endl;
#endif
    if (string(element->Name()) == "BinaryOp") {
        return create_binaryOp(element);
    }
    if (string(element->Name()) == "UnaryOp") {
        return create_unaryOp(element);
    }
    else if (string(element->Name()) == "ArrayExp") {
        return create_arrayExp(element);
    }
    else if (string(element->Name()) == "CallExp") {
        return create_callExp(element);
    }
    else if (string(element->Name()) == "ClassVar") {
        return create_classVar(element);
    }
    else if (string(element->Name()) == "BoolExp") {
        return create_boolExp(element);
    }
    else if (string(element->Name()) == "This") {
        return create_this(element);
    }
    else if (string(element->Name()) == "Length") {
        return create_length(element);
    }
    else if (string(element->Name()) == "Esc") {
        return create_esc(element);
    }
    else if (string(element->Name()) == "GetInt") {
        return create_getInt(element);
    }
    else if (string(element->Name()) == "GetCh") {
        return create_getCh(element);
    }
    else if (string(element->Name()) == "GetArray") {
        return create_getArray(element);
    }
    else if (string(element->Name()) == "IdExp") {
        return create_leafnode<IdExp>(element, "IdExp", "id", ATTR_TYPE::ID);
    }
    else if (string(element->Name()) == "IntExp") {
        return create_leafnode<IntExp>(element, "IntExp", "val", ATTR_TYPE::INT);
    }
    else if (string(element->Name()) == "OpExp") {
        return create_leafnode<OpExp>(element, "OpExp", "op", ATTR_TYPE::OP);
    }
    else { 
       // cout << "Error in dispatching: Unknown element in Exp: " << element->Name()<< endl;
        return nullptr; //fail to create an expression
    }
}

BinaryOp* create_binaryOp(XMLElement *element) {
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
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "OpExp" )
            op = create_leafnode<OpExp>(ce, "OpExp", "op", ATTR_TYPE::OP);
        else {
            Exp *e = create_exp(ce);
            if (exp1 == nullptr) exp1 = e;
            else if (exp2 == nullptr)  exp2 = e;
        }
        ce = ce->NextSiblingElement();
    }
    if (op == nullptr) {
        cerr << "Error: BinaryOp: Missing operator" << endl;
        return nullptr;
    }
    if (exp1 == nullptr || exp2 == nullptr ) {
        cerr << "Error: BinaryOp: Missing expression" << endl;
        return nullptr;
    }
    BinaryOp * bo = new BinaryOp(get_position(element), exp1, op, exp2);
    if (se != nullptr) global_semant_map->setSemant(bo, se); //add semant to map
    return bo;
}

UnaryOp* create_unaryOp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating UnaryOp" << endl;
#endif
    Exp *exp = nullptr;
    OpExp *op = nullptr;
    if (string(element->Name()) != "UnaryOp") {
        cerr << "Error: input Element is not UnaryOp" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "OpExp" )
            op = create_leafnode<OpExp>(ce, "OpExp", "op", ATTR_TYPE::OP);
        else {
            Exp *e = create_exp(ce);
            if (exp == nullptr) exp = e;
            else cerr << "Error: UnaryOp: More than one expression in UnaryOp" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    UnaryOp* uo = new UnaryOp(get_position(element), op, exp);
    if (se != nullptr) global_semant_map->setSemant(uo, se); //add semant to map
    return uo;
}

ArrayExp* create_arrayExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating ArrayExp" << endl;
#endif
    Exp *exp1 = nullptr;
    Exp *exp2 = nullptr;
    if (string(element->Name()) != "ArrayExp") {
        cerr << "Error: input Element is not ArrayExp" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (exp1 == nullptr) exp1 = e;
        else if (exp2 == nullptr) exp2 = e;
        ce = ce->NextSiblingElement();
    }
    ArrayExp * ae = new ArrayExp(get_position(element), exp1, exp2);
    if (se != nullptr) global_semant_map->setSemant(ae, se); //add semant to map
    return ae;
}

CallExp* create_callExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating CallExp" << endl;
#endif
    IdExp *id = nullptr;
    Exp *exp = nullptr;
    ExpList *el = new ExpList();
    if (string(element->Name()) != "CallExp") {
        cerr << "Error: input Element is not CallExp" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "ParList" ) {
            el = create_exp_list(ce);
        } else {
            Exp *e = create_exp(ce);
            if (exp == nullptr) exp = e; //first expression must be the object expression
            else if (id == nullptr && e->getASTKind()==ASTKind::IdExp) //only if the ID is not the object, then it's the method ID
                id = static_cast<IdExp*>(e);
            else {
                cerr << "Error: CallExp: More than one expression in CallExp" << endl;
                return nullptr;
            }
        }
        ce = ce->NextSiblingElement();
    }
    if (exp == nullptr || id == nullptr) {
        cerr << "Error: CallExp: Missing expression or method id" << endl;
        return nullptr;
    }
    if (el != nullptr && el->size() == 0) {
        delete el; el = nullptr;
    }
    CallExp* cexp = new CallExp(get_position(element), exp, id, el);
    if (se != nullptr) global_semant_map->setSemant(cexp, se); //add semant to map
    return cexp;
}

ClassVar* create_classVar(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating ClassVar" << endl;
#endif
    Exp *exp = nullptr;
    IdExp *id = nullptr;
    if (string(element->Name()) != "ClassVar") {
        cerr << "Error: input Element is not ClassVar" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (exp == nullptr) exp = e;
        else if (id == nullptr && e->getASTKind()==ASTKind::IdExp) 
        //only if the ID is not the object, then it's the method ID
            id = static_cast<IdExp*>(e);
        else {
            cerr << "Error: ClassVar: More than one expression in ClassVar" << endl;
            return nullptr;
        }
        ce = ce->NextSiblingElement();
    }
    ClassVar* cv = new ClassVar(get_position(element), exp, id);
    if (se != nullptr) global_semant_map->setSemant(cv, se); //add semant to map
    return cv;
}

BoolExp* create_boolExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Bool" << endl;
#endif
    if (string(element->Name()) != "BoolExp") {
        cerr << "Error: input Element is not BoolExp" << endl;
        return nullptr;
    }   
    return create_leafnode<BoolExp>(element, "BoolExp", "val", ATTR_TYPE::BOOL);
}

This* create_this(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating This" << endl;
#endif
    if (string(element->Name()) != "This") {
        cerr << "Error: input Element is not This" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    This *t = new This(get_position(element));
    if (se != nullptr) global_semant_map->setSemant(t, se); //add semant to map
    return t;
}

Length* create_length(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating Length" << endl;
#endif
    Exp *exp = nullptr;
    if (string(element->Name()) != "Length") {
        cerr << "Error: input Element is not Length" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (exp == nullptr) exp = e;
        else cerr << "Error: Length: More than one expression in Length" << endl;
        ce = ce->NextSiblingElement();
    }
    Length *l = new Length(get_position(element), exp);
    if (se != nullptr) global_semant_map->setSemant(l, se); //add semant to map
    return l;
}

Esc* create_esc(XMLElement* element) {
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
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        if (string(ce->Name()) == "StmList" ) {
            sl = create_stm_list(ce);
        } else {
            Exp *e = create_exp(ce);
            if (e != nullptr) {
                if (exp == nullptr) exp = e;
                else cerr << "Error: Esc: More than one expression in Esc" << endl;
            } 
        }
        ce = ce->NextSiblingElement();
    }
    if (exp == nullptr) {
        cerr << "Error: Esc: No expression in Esc" << endl;
        return nullptr; //fail to get an expression
    }
    if (sl != nullptr && sl->size() == 0) {
        delete sl; sl = nullptr;
    }
    Esc *esc = new Esc(get_position(element), sl, exp);
    if (se != nullptr) global_semant_map->setSemant(esc, se); //add semant to map
    return esc;
}

GetInt* create_getInt(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating GetInt" << endl;
#endif
    if (string(element->Name()) != "GetInt") {
        cerr << "Error: input Element is not GetInt" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    GetInt *gi = new GetInt(get_position(element));
    if (se != nullptr) global_semant_map->setSemant(gi, se); //add semant to map
    return gi;
}

GetCh* create_getCh(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating GetCh" << endl;
#endif
    if (string(element->Name()) != "GetCh") {
        cerr << "Error: input Element is not GetCh" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    GetCh *gc = new GetCh(get_position(element));
    if (se != nullptr) global_semant_map->setSemant(gc, se); //add semant to map
    return gc;
}

GetArray* create_getArray(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating GetArray" << endl;
#endif
    Exp *exp = nullptr;
    if (string(element->Name()) != "GetArray") {
        cerr << "Error: input Element is not GetArray" << endl;
        return nullptr;
    }
    AST_Semant *se = get_semant(element); //read the semant info of the node
    XMLElement *ce = element->FirstChildElement();
    while (ce) {
        Exp *e = create_exp(ce);
        if (e != nullptr) {
            if (exp == nullptr) exp = e;
            else cerr << "Error: GetArray: More than one expression in GetArray" << endl;
        } else {
            cerr << "Error: GetArray: Unknown element in GetArray" << endl;
        }
        ce = ce->NextSiblingElement();
    }
    if (exp == nullptr) {
        cerr << "Error: GetArray: No expression in GetArray" << endl;
        return nullptr; //fail to get an expression
    }
    GetArray *ga = new GetArray(get_position(element), exp);
    if (se != nullptr) global_semant_map->setSemant(ga, se); //add semant to map
    return ga;
}

/*
IdExp* create_idExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating IdExp" << endl;
#endif
    if (element->Name() != "IdExp") {
        cerr << "Error: input Element is not IdExp" << endl;
        return nullptr;
    }
    return create_leafnode<IdExp>(element, "IdExp", "id", ATTR_TYPE::ID);
}

IntExp* create_intExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating IntExp" << endl;
#endif
    if (string(element->Name()) != "IntExp") {
        cerr << "Error: input Element is not IntExp" << endl;
        return nullptr;
    }
    return create_leafnode<IntExp>(element, "IntExp", "val", ATTR_TYPE::INT);
}

OpExp* create_opExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Creating OpExp" << endl;
#endif
    if (string(element->Name()) != "OpExp") {
        cerr << "Error: input Element is not Op" << endl;
        return nullptr;
    }
    return create_leafnode<OpExp>(element, "OpExp", "op", ATTR_TYPE::OP);
}


*/

Name_Maps* create_NameMaps(XMLElement* xml_root) {
    Name_Maps* name_maps = new Name_Maps();
    if (xml_root == nullptr) return nullptr;
#ifdef DEBUG
    std::cout << "Reading NameMaps" << endl;
#endif

    for (XMLElement* xml_class = xml_root->FirstChildElement("Class"); 
                xml_class != nullptr; xml_class = xml_class->NextSiblingElement("Class")) {
        string class_name = xml_class->Attribute("name");
        name_maps->add_class(class_name);
#ifdef DEBUG
        std::cout << "Creating Class: " << class_name << endl;
#endif
        const XMLAttribute* attr = xml_class->FirstAttribute();
        while (attr) {
            if (string(attr->Name()) == "ancestor") {
                string ancestor_name = attr->Value();
                name_maps->add_class_hiearchy(class_name, ancestor_name);
#ifdef DEBUG
            std::cout << "Adding Ancestor: " << ancestor_name << " to " << class_name << endl;
#endif
            }
            attr = attr->Next();
        }

        for (XMLElement* xml_var = xml_class->FirstChildElement("Var"); 
                xml_var != nullptr; xml_var = xml_var->NextSiblingElement("Var")) {
            string var_name = xml_var->Attribute("id");
            XMLElement *var_decl_node = xml_var->FirstChildElement("VarDecl");
            if (var_decl_node == nullptr) {
                cerr << "Error: VarDecl not found for var " << var_name << " in class " << class_name << endl;
                continue;
            }
            VarDecl* var_decl = create_varDecl(var_decl_node);
            name_maps->add_class_var(class_name, var_name, var_decl);
#ifdef DEBUG
        cout << "Reading var " << var_name << " for " << class_name << endl;
#endif
        }

        for (XMLElement* xml_method = xml_class->FirstChildElement("Method"); 
                xml_method != nullptr; xml_method = xml_method->NextSiblingElement("Method")) {
            string method_name = xml_method->Attribute("name");
            name_maps->add_method(class_name, method_name);
#ifdef DEBUG
            cout << "Reading method " << method_name << " for " << class_name << endl;
#endif
            vector<string> formal_names; // List to store formal names
            for (XMLElement* xml_formal = xml_method->FirstChildElement("Formal"); 
                    xml_formal != nullptr; xml_formal = xml_formal->NextSiblingElement("Formal")) {
                string formal_name = xml_formal->Attribute("id");
#ifdef DEBUG
                cout << "Reading formal " << formal_name << " for " << method_name << " in " << class_name << endl;
#endif
                XMLElement *formal_node = xml_formal->FirstChildElement("Formal");
                if (formal_node == nullptr) {
                    cerr << "Error: Formal not found for formal " << formal_name << " in method " << method_name << " in class " << class_name << endl;
                    continue;
                }
                Formal* formal = create_formal(formal_node);
#ifdef DEBUG
                cout << "Formal created for " << formal->id->id << " with type " << type_kind_string(formal->type->typeKind) << endl;
#endif
                name_maps->add_method_formal(class_name, method_name, formal_name, formal);
                formal_names.push_back(formal_name); // Add formal_name to the list
            }
            // Add the list of formal names to methodFormalList
            name_maps->add_method_formal_list(class_name, method_name, formal_names);

            for (XMLElement* xml_var = xml_method->FirstChildElement("Var"); 
                    xml_var != nullptr; xml_var = xml_var->NextSiblingElement("Var")) {
                string var_name = xml_var->Attribute("id");
                XMLElement *var_decl_node = xml_var->FirstChildElement("VarDecl");
                VarDecl* var_decl = create_varDecl(var_decl_node);
                name_maps->add_method_var(class_name, method_name, var_name, var_decl);
#ifdef DEBUG
                cout << "Reading var " << var_name << " for " << method_name << " in " << class_name << endl;
#endif
            }
        }
    }
#ifdef DEBUG
    cout << "NameMaps created" << endl;
#endif
    return name_maps;
}