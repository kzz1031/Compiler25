    #define DEBUG
    #undef DEBUG

    //This file defines all the AST node clone methods 
    #include <string>
    #include <vector>
    #include <variant>
    #include "ASTheader.hh"
    #include "FDMJAST.hh"

    using namespace std;
    using namespace fdmj;
    
    namespace fdmj {
    
    string ASTKind_string(ASTKind kind) {
        switch (kind) {
            case ASTKind::Program: return "Program";
            case ASTKind::MainMethod: return "MainMethod";
            case ASTKind::ClassDecl: return "ClassDecl";
            case ASTKind::Type: return "Type";
            case ASTKind::VarDecl: return "VarDecl";
            case ASTKind::MethodDecl: return "MethodDecl";
            case ASTKind::Formal: return "Formal";
            case ASTKind::Nested: return "Nested";
            case ASTKind::If: return "If";
            case ASTKind::While: return "While";
            case ASTKind::Assign: return "Assign";
            case ASTKind::CallStm: return "CallStm";
            case ASTKind::Continue: return "Continue";
            case ASTKind::Break: return "Break";
            case ASTKind::Return: return "Return";
            case ASTKind::PutInt: return "PutInt";
            case ASTKind::PutCh: return "PutCh";
            case ASTKind::PutArray: return "PutArray";
            case ASTKind::Starttime: return "Starttime";
            case ASTKind::Stoptime: return "Stoptime";
            case ASTKind::BinaryOp: return "BinaryOp";
            case ASTKind::UnaryOp: return "UnaryOp";
            case ASTKind::ArrayExp: return "ArrayExp";
            case ASTKind::CallExp: return "CallExp";
            case ASTKind::ClassVar: return "ClassVar";
            case ASTKind::BoolExp: return "BoolExp";
            case ASTKind::This: return "This";
            case ASTKind::Length: return "Length";
            case ASTKind::Esc: return "Esc";
            case ASTKind::GetInt: return "GetInt";
            case ASTKind::GetCh: return "GetCh";
            case ASTKind::GetArray: return "GetArray";
            case ASTKind::IdExp: return "IdExp";
            case ASTKind::OpExp: return "OpExp";
            case ASTKind::IntExp: return "IntExp";
            default: return "Unknown";
        }
    }

    string type_kind_string(TypeKind typekind) {
        switch (typekind) {
            case TypeKind::INT:
                return "INT";
            case TypeKind::CLASS:
                return "CLASS";
            case TypeKind::ARRAY:
                return "INTARRAY";
            default:
                return "UNKNOWN";
        }
    }
    
    template <class T> vector<T*>* cloneList(vector<T*>* tl) {
        if (tl == nullptr || tl->size() == 0) return nullptr;
        vector<T*> *v = new vector<T*>();
        for (auto x : *tl) {
            ASTKind i=x->getASTKind();
            v->push_back(static_cast<T*>(x->clone()));
        }
        return v;
    }

    } //namespace fdmj

    Program* Program::clone() {
        MainMethod *m = (this->main)? static_cast<MainMethod*>(main->clone()): nullptr;
        vector<ClassDecl*> *cdl_clone = (this->cdl)? cloneList<ClassDecl>(this->cdl): nullptr;
        return new Program(pos->clone(), m, cdl_clone);
    }

    MainMethod* MainMethod::clone() {
        vector<VarDecl*> *vdl_clone = (this->vdl)? cloneList<VarDecl>(this->vdl): nullptr;
        vector<Stm*> *sl_clone = (this->sl)? cloneList<Stm>(this->sl): nullptr;
        return new MainMethod(pos->clone(), vdl_clone, sl_clone);
    }

    ClassDecl* ClassDecl::clone() {
        IdExp *id_clone = (this->id)? static_cast<IdExp*>(this->id->clone()): nullptr;
        IdExp *eid_clone = (this->eid)? static_cast<IdExp*>(this->eid->clone()): nullptr;
        vector<VarDecl*> *vdl_clone = (this->vdl)? cloneList<VarDecl>(this->vdl): nullptr;
        vector<MethodDecl*> *mdl_clone = (this->mdl)? cloneList<MethodDecl>(this->mdl): nullptr;
        return new ClassDecl(pos->clone(), id_clone, eid_clone, vdl_clone, mdl_clone);
    }

    Type* Type::clone() {
        return new Type(pos->clone(), typeKind, (cid)? static_cast<IdExp*>(cid->clone()): nullptr, arity);
    }

    VarDecl* VarDecl::clone() {
        IdExp *id_clone = (this->id != nullptr)? static_cast<IdExp*>(id->clone()): nullptr;
        if (holds_alternative<IntExp*>(init)) {
            return new VarDecl(pos->clone(), type->clone(), id_clone, get<IntExp*>(init)->clone());
        } else if (holds_alternative<vector<IntExp*>*>(init)) {
            return new VarDecl(pos->clone(), type->clone(), id_clone, cloneList<IntExp>(get<vector<IntExp*>*>(init)));
        } else {
            return new VarDecl(pos->clone(), type->clone(), id_clone);
        }
    }

    MethodDecl* MethodDecl::clone() {
        Type *t_clone = (this->type)? static_cast<Type*>(type->clone()): nullptr;
        IdExp *id_clone = (this->id)? static_cast<IdExp*>(id->clone()): nullptr;
        vector<Formal*> *fl_clone = (this->fl)? cloneList<Formal>(this->fl): nullptr;
        vector<VarDecl*> *vdl_clone = (this->vdl)? cloneList<VarDecl>(this->vdl): nullptr;
        vector<Stm*> *sl_clone = (this->sl)? cloneList<Stm>(this->sl): nullptr;
        return new MethodDecl(pos->clone(), t_clone, id_clone, fl_clone, vdl_clone, sl_clone);
    }

    Formal* Formal::clone() {
        Type *t_clone = (this->type)? static_cast<Type*>(type->clone()): nullptr;
        IdExp *id_clone = (this->id)? static_cast<IdExp*>(id->clone()): nullptr;
        return new Formal(pos->clone(), t_clone, id_clone);
    }

    Nested* Nested::clone() {
        vector<Stm*> *s_clone = cloneList<Stm>(this->sl);
        return new Nested(pos->clone(), s_clone);
    }

    If* If::clone() {
        Exp *e_clone = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        Stm *s1_clone = (this->stm1)? static_cast<Stm*>(stm1->clone()): nullptr;
        Stm *s2_clone = (this->stm2)? static_cast<Stm*>(stm2->clone()): nullptr;
        return new If(pos->clone(), e_clone, s1_clone, s2_clone);
    }

    While* While::clone() {
        Exp *e_clone = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        Stm *s_clone = (this->stm)? static_cast<Stm*>(stm->clone()): nullptr;
        return new While(pos->clone(), e_clone, s_clone);
    }

    Assign* Assign::clone() {
        Exp *l = (this->left)? static_cast<Exp*>(left->clone()): nullptr;
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        return new Assign(pos->clone(), l, e);
    }

    CallStm* CallStm::clone() {
        Exp *e = (this->obj)? static_cast<Exp*>(obj->clone()): nullptr;
        IdExp *i = (this->name)? static_cast<IdExp*>(name->clone()): nullptr;
        vector<Exp*> *p = (this->par)? cloneList<Exp>(this->par): nullptr;
        return new CallStm(pos->clone(), e, i, p);
    }

    Continue* Continue::clone() {
        return new Continue(pos->clone());
    }

    Break* Break::clone() {
        return new Break(pos->clone());
    }

    Return* Return::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        return new Return(pos->clone(), e);
    }

    PutInt* PutInt::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        return new PutInt(pos->clone(), e);
    }

    PutCh* PutCh::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        return new PutCh(pos->clone(), e);
    }

    PutArray* PutArray::clone() {
        Exp *a = (this->arr)? static_cast<Exp*>(arr->clone()): nullptr;
        Exp *i = (this->n)? static_cast<Exp*>(n->clone()): nullptr;
        return new PutArray(pos->clone(), n, arr);
    }

    Starttime* Starttime::clone() {
        return new Starttime(pos->clone());
    }

    Stoptime* Stoptime::clone() {
        return new Stoptime(pos->clone());
    }

    BinaryOp* BinaryOp::clone() {
        Exp *l = (this->left)? static_cast<Exp*>(left->clone()): nullptr;
        Exp *r = (this->right)? static_cast<Exp*>(right->clone()): nullptr;
        OpExp *o = (this->op)? static_cast<OpExp*>(op->clone()): nullptr;
        return new BinaryOp(pos->clone(), l, op, r);
    }

    UnaryOp* UnaryOp::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        OpExp *o = (this->op)? static_cast<OpExp*>(op->clone()): nullptr;
        return new UnaryOp(pos->clone(), op, e);
    }

    ArrayExp* ArrayExp::clone() {
        Exp *a = (this->arr)? static_cast<Exp*>(arr->clone()): nullptr;
        Exp *i = (this->index)? static_cast<Exp*>(index->clone()): nullptr;
        return new ArrayExp(pos->clone(), a, i);
    }

    CallExp* CallExp::clone() {
        Exp *o = (this->obj)? static_cast<Exp*>(obj->clone()): nullptr;
        IdExp *n = (this->name)? static_cast<IdExp*>(name->clone()): nullptr;
        vector<Exp*> *p = (this->par)? cloneList<Exp>(this->par): nullptr;
        return new CallExp(pos->clone(), o, n, p);
    }

    ClassVar* ClassVar::clone() {
        Exp *o = (this->obj)? static_cast<Exp*>(obj->clone()): nullptr;
        IdExp *i = (this->id)? static_cast<IdExp*>(id->clone()): nullptr;
        return new ClassVar(pos->clone(), o, i);
    }

    BoolExp* BoolExp::clone() {
        return new BoolExp(pos->clone(), val);
    }

    This* This::clone() {
        return new This(pos->clone());
    }

    Length* Length::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        return new Length(pos->clone(), e);
    }

    Esc* Esc::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        vector<Stm*> *s = (this->sl)? cloneList<Stm>(this->sl): nullptr;
        return new Esc(pos->clone(), s, e);
    }

    GetInt* GetInt::clone() {
        return new GetInt(pos->clone());
    }

    GetCh* GetCh::clone() {
        return new GetCh(pos->clone());
    }

    GetArray* GetArray::clone() {
        Exp *e = (this->exp)? static_cast<Exp*>(exp->clone()): nullptr;
        return new GetArray(pos->clone(), e);
    }

    IdExp* IdExp::clone() {
        return new IdExp(pos->clone(), this->id);
    }

    IntExp* IntExp::clone() {
        return new IntExp(pos->clone(), val);
    }

    OpExp* OpExp::clone() {
        return new OpExp(pos->clone(), op);
    }