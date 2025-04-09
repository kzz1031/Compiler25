    #ifndef _FDMJAST_HH
    #define _FDMJAST_HH

    //This file defines all the AST node classes

    #include <string>
    #include <vector>
    #include "ASTheader.hh"
    #include "FDMJAST.hh"

    using namespace std;

    namespace fdmj {
    
    class Program : public AST {
    public:
        MainMethod *main;
        vector<ClassDecl*> *cdl = new vector<ClassDecl*>();
        Program(Pos* pos, MainMethod *main): AST(pos), main(main), cdl(nullptr) {};
        Program(Pos* pos, MainMethod *main, vector<ClassDecl*> *cdl): 
                AST(pos), main(main), cdl(cdl) {};
        ASTKind getASTKind() override {return ASTKind::Program;}
        Program* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);}
    };

    class MainMethod : public AST {
    public:
        vector<VarDecl*> *vdl;
        vector<Stm*> *sl;
        MainMethod(Pos *pos, vector<VarDecl*> *vdl, vector<Stm*> *sl): 
            AST(pos), vdl(vdl), sl(sl) {}
        ASTKind getASTKind() override {return ASTKind::MainMethod;}
        MainMethod* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);}
    };

    class ClassDecl: public AST {
    public:
        IdExp *id = nullptr;
        IdExp *eid = nullptr; // nullptr if not assigned
        vector<VarDecl*> *vdl = new vector<VarDecl*>();
        vector<MethodDecl*> *mdl = new vector<MethodDecl*>();
        ClassDecl(Pos *pos, IdExp* id, vector<VarDecl*> *vdl, vector<MethodDecl*> *mdl):
                AST(pos), id(id), vdl(vdl), mdl(mdl) {}
        ClassDecl(Pos *pos, IdExp* id, IdExp* eid, vector<VarDecl*> *vdl, vector<MethodDecl*> *mdl):
                AST(pos), id(id), eid(eid), vdl(vdl), mdl(mdl) {}
        ASTKind getASTKind() override {return ASTKind::ClassDecl;}
        ClassDecl* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);} 
    };

    class Type : public AST {
    public:
        TypeKind typeKind;
        IdExp *cid = nullptr;  //class id
        IntExp *arity = nullptr;  //array arity
        Type(Pos *pos): AST(pos), typeKind(TypeKind::INT) {}
        Type(Pos *pos, IdExp *cid): 
            AST(pos), typeKind(TypeKind::CLASS), cid(cid) {}
        Type(Pos *pos, IntExp *arity): 
            AST(pos), typeKind(TypeKind::ARRAY), arity(arity) {} //array must have arity=0
        Type(Pos *pos, TypeKind typeKind, IdExp *cid, IntExp *arity): 
            AST(pos), typeKind(typeKind), cid(cid), arity(arity) {}
        ASTKind getASTKind() override {return ASTKind::Type;}
        Type* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);}
    };

    class VarDecl : public AST {
    public:
        Type *type = nullptr;
        IdExp  *id = nullptr;
        variant<monostate, IntExp*, vector<IntExp*>*> init;
        //note that nullptr means no init. vector.size=0 means empty array initialization
        VarDecl(Pos *pos, Type *type, IdExp  *id): 
                AST(pos), type(type), id(id) {init = std::monostate{};}
        VarDecl(Pos *pos, Type *type, IdExp  *id, IntExp *init_int): 
                AST(pos), type(type), id(id), init(init_int) {}
        VarDecl(Pos *pos, Type *type, IdExp  *id, vector<IntExp*> *init_array):
                AST(pos), type(type), id(id), init(init_array) {}
        VarDecl(Pos *pos, Type *type, IdExp *id, variant<monostate, IntExp*, vector<IntExp*>*> init): 
                AST(pos), type(type), id(id), init(init) {}
        ASTKind getASTKind() override {return ASTKind::VarDecl;}
        VarDecl* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);}
    };

    class MethodDecl : public AST {
    public:
        Type *type = nullptr;
        IdExp  *id = nullptr; //id expression
        vector<Formal*> *fl = new vector<Formal*>();
        vector<VarDecl*> *vdl = new vector<VarDecl*>();
        vector<Stm*> *sl =  new vector<Stm*>();
        MethodDecl(Pos *pos, Type *type, IdExp *id, vector<Formal*> *fl, vector<VarDecl*> *vdl, vector<Stm*> *sl):
                AST(pos), type(type), id(id), fl(fl), vdl(vdl), sl(sl) {}
        MethodDecl(Pos *pos, Type *type, IdExp *id, vector<VarDecl*> *vdl, vector<Stm*> *sl): //without formalList
                AST(pos), type(type), id(id), vdl(vdl), sl(sl) {}
        MethodDecl(Pos *pos, Type *type, IdExp *id, vector<Formal*> *fl, vector<Stm*> *sl): //without varDeclList
                AST(pos), type(type), id(id), fl(fl), sl(sl) {}
        MethodDecl(Pos *pos, Type *type, IdExp *id, vector<Stm*> *sl): //without formalList and varDeclList
                AST(pos), type(type), id(id), sl(sl) {}
        ASTKind getASTKind() override {return ASTKind::MethodDecl;}
        MethodDecl* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);}
    };

    class Formal : public AST {
    public:
        Type *type = nullptr;
        IdExp *id = nullptr;
        Formal(Pos *pos, Type *type, IdExp *id): AST(pos), type(type), id(id) {
            if (type->typeKind == TypeKind::ARRAY) {
                if (type->arity == nullptr) {
                    cerr << "at position: " << pos->print() << endl;
                    cerr << "Error: Array type has no arity in the formal. Not allowed!" << endl;
                    exit(1);
                }
            }
        }
        ASTKind getASTKind() override {return ASTKind::Formal;}
        Formal* clone() override;
        void accept(AST_Visitor &v) override {v.visit(this);}
    };
    
    class Stm : public AST { //this is the class for all statements
    public:
        Stm(Pos *pos): AST(pos) {}
        virtual ASTKind getASTKind()=0;
    };

    class Nested : public Stm {
    public:
        vector<Stm*>* sl;
        Nested() = default;
        Nested(Pos *pos, vector<Stm*>* sl): Stm(pos), sl(sl) {}
        ASTKind getASTKind() override {return ASTKind::Nested;}
        Nested* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class If : public Stm {
    public:
        Exp *exp = nullptr;
        Stm *stm1 = nullptr;
        Stm *stm2 = nullptr; //else part, could be empty
        If(Pos *pos, Exp *exp, Stm *stm1, Stm *stm2): Stm(pos), exp(exp), stm1(stm1), stm2(stm2) {}
        If(Pos *pos, Exp *exp, Stm *stm1): Stm(pos), exp(exp), stm1(stm1), stm2(nullptr) {}
        ASTKind getASTKind() override {return ASTKind::If;}
        If* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class While : public Stm {
    public:
        Exp *exp = nullptr;
        Stm *stm = nullptr; //body of the while loop, could be empty
        While(Pos *pos, Exp *exp, Stm *stm): Stm(pos), exp(exp), stm(stm) {}
        While(Pos *pos, Exp *exp): Stm(pos), exp(exp), stm(nullptr) {}
        ASTKind getASTKind() override {return ASTKind::While;}
        While* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Assign : public Stm {
    public:
        Exp *left = nullptr;
        Exp *exp = nullptr;
        Assign(Pos *pos, Exp *left, Exp *exp): Stm(pos), left(left), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::Assign;}
        Assign* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class CallStm : public Stm {
    public:
        Exp *obj = nullptr;
        IdExp *name = nullptr;
        vector<Exp*> *par = new vector<Exp*>();

        CallStm(Pos *pos, Exp *obj, IdExp *name, vector<Exp*> *par): Stm(pos), obj(obj), name(name), par(par) {}
        CallStm(Pos *pos, Exp *obj, IdExp *name): Stm(pos), obj(obj), name(name), par(nullptr) {}
        
        ASTKind getASTKind() override {return ASTKind::CallStm;}
        CallStm* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Continue : public Stm {
    public:
        Continue(Pos *pos): Stm(pos) {}
        ASTKind getASTKind() override {return ASTKind::Continue;}
        Continue* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Break : public Stm {
    public:
        Break(Pos *pos): Stm(pos) {}
        ASTKind getASTKind() override {return ASTKind::Break;}
        Break* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Return : public Stm {
    public:
        Exp *exp = nullptr;
        Return(Pos *pos, Exp *exp): Stm(pos), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::Return;}
        Return* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class PutInt : public Stm {
    public:
        Exp *exp = nullptr;
        PutInt(Pos *pos, Exp *exp): Stm(pos), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::PutInt;}
        PutInt* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class PutCh : public Stm {
    public:
        Exp *exp = nullptr;

        PutCh(Pos *pos, Exp *exp): Stm(pos), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::PutCh;}
        PutCh* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class PutArray : public Stm { 
    public:
        Exp *n= nullptr; //size to print (from beginning of the following array)
        Exp *arr = nullptr; //the arary

        PutArray(Pos *pos, Exp *n, Exp *arr): Stm(pos), n(n), arr(arr) {}
        ASTKind getASTKind() override {return ASTKind::PutArray;}
        PutArray* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Starttime : public Stm {
    public:
        Starttime(Pos *pos): Stm(pos) {}
        ASTKind getASTKind() override {return ASTKind::Starttime;}
        Starttime* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Stoptime : public Stm {
    public:
        Stoptime(Pos *pos): Stm(pos) {}
        ASTKind getASTKind() override {return ASTKind::Stoptime;}
        Stoptime* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Exp : public AST { //this is the class for all expressions
    public:
        Exp(Pos *pos): AST(pos) {}
        virtual ASTKind getASTKind()=0;
    };

    class BinaryOp : public Exp {
    public:
        Exp *left = nullptr;
        OpExp *op = nullptr;
        Exp *right = nullptr;
        BinaryOp(Pos *pos, Exp *left, OpExp *op, Exp *right): Exp(pos), left(left), op(op), right(right) {}
        ASTKind getASTKind() override {return ASTKind::BinaryOp;}
        BinaryOp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class UnaryOp : public Exp {
    public:
        OpExp *op = nullptr;
        Exp *exp = nullptr;
        UnaryOp(Pos *pos, OpExp *op, Exp *exp): Exp(pos), op(op), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::UnaryOp;}
        UnaryOp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };
    
    class ArrayExp : public Exp {
    public:
        Exp *arr = nullptr;
        Exp *index = nullptr;
        ArrayExp(Pos *pos, Exp *arr, Exp *index): Exp(pos), arr(arr), index(index) {}
        ASTKind getASTKind() override {return ASTKind::ArrayExp;}
        ArrayExp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class CallExp : public Exp {
    public:
        Exp *obj = nullptr;
        IdExp *name = nullptr;
        vector<Exp*> *par = new vector<Exp*>();
    
        CallExp(Pos *pos, Exp *obj, IdExp *name, vector<Exp*> *par): Exp(pos), obj(obj), name(name), par(par) {}
        CallExp(Pos *pos, Exp *obj, IdExp *name): Exp(pos), obj(obj), name(name), par(nullptr) {}
        ASTKind getASTKind() override {return ASTKind::CallExp;}
        CallExp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class ClassVar : public Exp {
    public:
        Exp *obj = nullptr;
        IdExp *id = nullptr;

        ClassVar(Pos *pos, Exp *obj, IdExp *id): Exp(pos), obj(obj), id(id) {}
        ASTKind getASTKind() override {return ASTKind::ClassVar;}
        ClassVar* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class BoolExp: public Exp {
    public:
        bool val;
        BoolExp(Pos *pos, bool val): Exp(pos), val(val) {}
        ASTKind getASTKind() override {return ASTKind::BoolExp;}
        BoolExp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class This : public Exp {
    public:
        This(Pos *pos): Exp(pos) {}
        ASTKind getASTKind() override {return ASTKind::This;}
        This* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Length : public Exp {
    public:
        Exp *exp = nullptr;
        Length(Pos *pos, Exp *exp): Exp(pos), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::Length;}
        Length* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class Esc : public Exp {
    public:
        vector<Stm*> *sl = new vector<Stm*>();
        Exp *exp = nullptr;

        Esc(Pos *pos, vector<Stm*> *sl, Exp *exp): Exp(pos), sl(sl), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::Esc;}
        Esc* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class GetInt : public Exp {
    public:
        GetInt(Pos *pos): Exp(pos) {}
        ASTKind getASTKind() override {return ASTKind::GetInt;}
        GetInt* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class GetCh : public Exp {
    public:
        GetCh(Pos *pos): Exp(pos) {}
        ASTKind getASTKind() override {return ASTKind::GetCh;}
        GetCh* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class GetArray : public Exp {
    public:
        Exp *exp = nullptr;
        GetArray(Pos *pos, Exp *exp): Exp(pos), exp(exp) {}
        ASTKind getASTKind() override {return ASTKind::GetArray;}
        GetArray* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class IdExp : public Exp {
    public:
        string id;
        IdExp(Pos *pos, string id): Exp(pos), id(id) {}
        ASTKind getASTKind() override {return ASTKind::IdExp;}
        IdExp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class IntExp : public Exp {
    public:
        int val;
        IntExp(Pos *pos, int val): Exp(pos), val(val) {}
        ASTKind getASTKind() override {return ASTKind::IntExp;}
        IntExp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };

    class OpExp : public Exp {
    public:
        string op;
        OpExp(Pos *pos, string op): Exp(pos), op(op) {}
        ASTKind getASTKind() override {return ASTKind::OpExp;}
        OpExp* clone() override;
        void accept(AST_Visitor &v) override {
            v.visit(this);
        }
    };


}// namespace fdmj

#endif