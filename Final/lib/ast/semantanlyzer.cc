#define DEBUG
#undef DEBUG

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "namemaps.hh"
#include "semant.hh"

using namespace std;
using namespace fdmj;

#ifdef DEBUG
#define DEBUG_PRINT(msg) std::cerr << msg << std::endl
#else
#define DEBUG_PRINT(msg)
#endif

static string current_class = "";
static string current_method = "";
static bool in_loop = false;  // For continue/break check
static TypeKind current_return_type = TypeKind::INT; // Default to INT
static int loop_depth = 0;    // Track nested loop depth

// Helper functions for type checking
bool check_compatible_types(TypeKind t1, variant<monostate,string,int> p1,
                          TypeKind t2, variant<monostate,string,int> p2,
                          Name_Maps* name_maps) {
    DEBUG_PRINT("Checking type compatibility: Type1=" << static_cast<int>(t1) << " Type2=" << static_cast<int>(t2));
    try {
        // Special case: if either type is monostate, allow the comparison
        if (holds_alternative<monostate>(p1) || holds_alternative<monostate>(p2)) {
            return t1 == t2;
        }

        if (t1 == t2) {
            switch(t1) {
                case TypeKind::CLASS: {
                    if (!holds_alternative<string>(p1) || !holds_alternative<string>(p2)) {
                        return false;
                    }
                    string c1 = get<string>(p1);
                    string c2 = get<string>(p2);
                    if (c1 == c2) return true;
                    auto ancestors = name_maps->get_ancestors(c2);
                    return ancestors && std::find(ancestors->begin(), ancestors->end(), c1) != ancestors->end();
                }
                case TypeKind::ARRAY: {
                    if (!holds_alternative<int>(p1) || !holds_alternative<int>(p2)) {
                        return true;  // For simple array comparison, don't check dimensions
                    }
                    return get<int>(p1) == get<int>(p2);
                }
                default:
                    return true;
            }
        }
        return false;
    } catch (const std::bad_variant_access& e) {
        DEBUG_PRINT("Type checking error: Invalid type parameters (" 
                    << static_cast<int>(t1) << ", " << static_cast<int>(t2) << ")");
        return false;
    }
}

AST_Semant_Map* semant_analyze(Program* node) {
    printf("semant_analyzeee\n");
    if (node == nullptr) {
        printf("node is null\n");
        return nullptr;
    }
    Name_Maps* name_maps = makeNameMaps(node);
    AST_Semant_Visitor semant_visitor(name_maps);
    semant_visitor.visit(node);
    std::cerr << "Semantic Analysis Doneeee" << std::endl << std::endl;
    return semant_visitor.getSemantMap();
}

void AST_Semant_Visitor::visit(Program* node) {
    DEBUG_PRINT("\n=== Visiting Program ===");
#ifdef DEBUG
    std::cout << "Visiting Program" << std::endl;
#endif
    if (node == nullptr) {
        return;
    }
    if (node->main != nullptr) {
        node->main->accept(*this);
    }
    if (node->cdl != nullptr) {
        for (auto cl : *(node->cdl)) {
            cl->accept(*this);
        }
    }
}

void AST_Semant_Visitor::visit(MainMethod* node) {
    DEBUG_PRINT("\n=== Visiting MainMethod ===");
    if (node == nullptr) return;
    current_class = "_^main^_";
    current_method = "main";
    current_return_type = TypeKind::INT;
    
    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }
    
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            s->accept(*this);
        }
    }
}

void AST_Semant_Visitor::visit(ClassDecl* node) {
    DEBUG_PRINT("\n=== Visiting ClassDecl: " << (node ? node->id->id : "null"));
    if (node == nullptr) return;
    
    current_class = node->id->id;
    current_method = "";

    // check method overriding
    if (node->mdl != nullptr) {
        for (auto method : *(node->mdl)) {
            string method_name = method->id->id;
            
            auto ancestors = name_maps->get_ancestors(current_class);
            
            for (const auto& ancestor_class : *ancestors) {
                if (name_maps->is_method(ancestor_class, method_name)) {
                    DEBUG_PRINT("Found overridden method: " << method_name 
                              << " in ancestor class: " << ancestor_class);
                    
                    auto parent_formals = name_maps->get_method_formal_list(ancestor_class, method_name);
                    if (!parent_formals) continue;

                    vector<Formal*>* child_formals = method->fl;
     
                    Formal* parent_return = (*parent_formals)[parent_formals->size()-1];
                    if (method->type->typeKind != parent_return->type->typeKind || 
                        (method->type->typeKind == TypeKind::CLASS && 
                         method->type->cid->id != parent_return->type->cid->id)) {
                        cerr << "Error at " << method->getPos()->print() 
                             << ": Method '" << method_name 
                             << "' in class '" << current_class 
                             << "' has different return type from ancestor class '" 
                             << ancestor_class << "'" << endl;
                        break;
                    }
                    
                    if (child_formals->size() != parent_formals->size() - 1 ) {
                        cerr << "Error at " << method->getPos()->print() 
                             << ": Method '" << method_name 
                             << "' in class '" << current_class 
                             << "' has different number of parameters from ancestor class '" 
                             << ancestor_class << "'" << endl;
                        break;
                    }
                    
                    bool param_mismatch = false;
                    for (size_t i = 0; i < child_formals->size(); i++) {
                        auto child_formal = (*child_formals)[i];
                        auto parent_formal = (*parent_formals)[i];
                        
                        if (child_formal->type->typeKind != parent_formal->type->typeKind ||
                            (child_formal->type->typeKind == TypeKind::CLASS &&
                             child_formal->type->cid->id != parent_formal->type->cid->id) ||
                            (child_formal->type->typeKind == TypeKind::ARRAY &&
                             child_formal->type->arity->val != parent_formal->type->arity->val)) {
                            
                            cerr << "Error at " << method->getPos()->print() 
                                 << ": Parameter " << (i+1) << " of method '" << method_name 
                                 << "' in class '" << current_class 
                                 << "' has different type from ancestor class '" 
                                 << ancestor_class << "'" << endl;
                            param_mismatch = true;
                            break;
                        }
                    }
                    
                    if (param_mismatch) break;
                }
            }
        }
    }

    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }
    
    if (node->mdl != nullptr) {
        for (auto md : *(node->mdl)) {
            md->accept(*this);
        }
    }
}

void AST_Semant_Visitor::visit(MethodDecl* node) {
    DEBUG_PRINT("\n=== Visiting MethodDecl: " << (node ? node->id->id : "null"));
    if (node == nullptr) return;
    
    current_method = node->id->id;
    
    // Set return type
    if (node->type->typeKind == TypeKind::CLASS) {
        current_return_type = TypeKind::CLASS;
    } else {
        current_return_type = node->type->typeKind;
    }

    // Process parameters
    if (node->fl != nullptr) {
        for (auto f : *(node->fl)) {
            f->accept(*this);
        }
    }

    // Process local variables
    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }

    // Process statements
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            s->accept(*this);
        }
    }
}

void AST_Semant_Visitor::visit(Assign* node) {
    if (node == nullptr) return;
    
    // Visit left and right sides
    node->left->accept(*this);
    node->exp->accept(*this);
    
    // Get semantic info
    auto left_sem = semant_map->getSemant(node->left);
    auto right_sem = semant_map->getSemant(node->exp);
    
    if (!left_sem->is_lvalue()) {
        cerr << "Error at " << node->getPos()->print() << ": Left side of assignment must be an lvalue" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Special handling for class types - allow upcasting
    if (left_sem->get_type() == TypeKind::CLASS && right_sem->get_type() == TypeKind::CLASS) {
        string left_class = get<string>(left_sem->get_type_par());
        string right_class = get<string>(right_sem->get_type_par());
        
        // 如果左右类型相同，或右边类型是左边类型的子类（即左边类型是右边类型的祖先），则允许赋值
        auto ancestors = name_maps->get_ancestors(right_class);
        if (left_class == right_class ||
            (ancestors && std::find(ancestors->begin(), ancestors->end(), left_class) != ancestors->end())) {
            return;
        }
        
        cerr << "Error at " << node->getPos()->print() 
             << ": Invalid class assignment - cannot assign " << right_class 
             << " to " << left_class << endl;
        exit(EXIT_FAILURE);
    }
    
    // For non-class types, check exact type match
    if (!check_compatible_types(left_sem->get_type(), left_sem->get_type_par(),
                              right_sem->get_type(), right_sem->get_type_par(),
                              name_maps)) {
        cerr << "Error at " << node->getPos()->print() << ": Type mismatch in assignment" << endl;
        cerr << "Left side: " << type_kind_string(left_sem->get_type()) << endl;
        cerr << "Right side: " << type_kind_string(right_sem->get_type()) << endl;
        exit(EXIT_FAILURE);
    }
}

void AST_Semant_Visitor::visit(While* node) {
    DEBUG_PRINT("\n=== Visiting While ===");
    if (node == nullptr) return;
    
    // Process condition
    node->exp->accept(*this);
    auto cond_sem = semant_map->getSemant(node->exp);
    
    // Check if condition is boolean
    if (cond_sem->get_type() != TypeKind::INT) {
        cerr << "Error at " << node->getPos()->print() << ": While condition must be boolean" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Enter loop context
    loop_depth++;
    bool was_in_loop = in_loop;
    in_loop = true;
    
    // Process body
    if (node->stm != nullptr) {
        node->stm->accept(*this);
    }
    
    // Restore loop context
    loop_depth--;
    in_loop = was_in_loop;
    DEBUG_PRINT("While loop semantic check passed");
}

void AST_Semant_Visitor::visit(CallStm* node) {
    DEBUG_PRINT("\n=== Visiting CallStm ===");
    DEBUG_PRINT("name: " << node->name->id);
    if (node == nullptr) return;

    // Visit object and name
    if (node->obj == nullptr) {
        cerr << "Error at " << node->getPos()->print() << ": Null object in method call" << endl;
        exit(EXIT_FAILURE);
    }
    
    node->obj->accept(*this);
    auto obj_sem = semant_map->getSemant(node->obj);
    
    if (obj_sem == nullptr || obj_sem->get_type() != TypeKind::CLASS) {
        cerr << "Error at " << node->getPos()->print() 
             << ": Method call on non-class type" << endl;
        exit(EXIT_FAILURE);
    }
    
    string class_name = get<string>(obj_sem->get_type_par());
    string method_name = node->name->id;
    
    // 在当前类及其所有父类中查找方法
    string method_class = class_name;
    bool found_method = false;
    
    // 首先检查当前类
    if (name_maps->is_method(class_name, method_name)) {
        found_method = true;
        method_class = class_name;
    } else {
        // 检查所有父类
        auto ancestors = name_maps->get_ancestors(class_name);
        for (const auto& ancestor : *ancestors) {
            if (name_maps->is_method(ancestor, method_name)) {
                found_method = true;
                method_class = ancestor;
                break;
            }
        }
    }
    
    if (!found_method) {
        cerr << "Error at " << node->getPos()->print() 
             << ": Method '" << method_name 
             << "' not found in class '" << class_name 
             << "' or its ancestors" << endl;
        exit(EXIT_FAILURE);
    }
    
    // 使用找到方法的实际类来检查参数
    auto formal_list = name_maps->get_method_formal_list(method_class, method_name);
    if (formal_list == nullptr) {
        cerr << "Error: Could not get formal parameter list for method " << method_name << endl;
        exit(EXIT_FAILURE);
    }
    
    try {
        string class_name = get<string>(obj_sem->get_type_par());
        DEBUG_PRINT("Method call on class: " << class_name);
        DEBUG_PRINT("Method name: " << node->name->id);
        
        // Check parameters
        auto formal_list = name_maps->get_method_formal_list(method_class, node->name->id);
        if (formal_list == nullptr) {
            cerr << "Error: Could not get formal parameter list for method " << node->name->id << endl;
            exit(EXIT_FAILURE);
        }
        
        size_t param_count = (node->par != nullptr) ? node->par->size() : 0;
        if (formal_list->size() - 1 != param_count) { // -1 for return type
            cerr << "Error at " << node->getPos()->print() 
                 << ": Wrong number of parameters. Expected " << formal_list->size() - 1 
                 << ", got " << param_count << endl;
            exit(EXIT_FAILURE);
        }
        
        if (node->par != nullptr) {
            for (size_t i = 0; i < node->par->size(); i++) {
                auto param = (*node->par)[i];
                if (param == nullptr) continue;
                
                param->accept(*this);
                auto param_sem = semant_map->getSemant(param);
                if (param_sem == nullptr) {
                    cerr << "Error: Could not get semantic info for parameter " << i << endl;
                    continue;
                }
                
                auto formal = (*formal_list)[i];
                if (formal == nullptr || formal->type == nullptr) {
                    cerr << "Error: Invalid formal parameter at index " << i << endl;
                    continue;
                }
                
                variant<monostate,string,int> formal_type_par = 
                    formal->type->cid ? variant<monostate,string,int>(formal->type->cid->id) : monostate();
                
                if (!check_compatible_types(formal->type->typeKind, formal_type_par,
                                         param_sem->get_type(), param_sem->get_type_par(),
                                         name_maps)) {
                    cerr << "Error at " << node->getPos()->print() 
                         << ": Parameter type mismatch at position " << i << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
        
    } catch (const std::bad_variant_access& e) {
        cerr << "Error: Invalid type information in method call" << endl;
    }
    DEBUG_PRINT("Method call semantic check passed");

}

void AST_Semant_Visitor::visit(Return* node) {
    DEBUG_PRINT("\n=== Visiting Return ===");
    if (node == nullptr) return;

    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto exp_sem = semant_map->getSemant(node->exp);
        if (exp_sem == nullptr) {
            cerr << "Error: Could not get semantic info for return expression" << endl;
            exit(EXIT_FAILURE);
        }

        variant<monostate,string,int> method_return_type_par = monostate();
        if (current_return_type == TypeKind::CLASS) {
            auto formal_list = name_maps->get_method_formal_list(current_class, current_method);
            if (formal_list != nullptr && !formal_list->empty()) {
                auto return_formal = (*formal_list)[formal_list->size() - 1];
                if (return_formal->type->cid) {
                    method_return_type_par = return_formal->type->cid->id;
                }
            }
        }

        if (!check_compatible_types(current_return_type, method_return_type_par,
                                  exp_sem->get_type(), exp_sem->get_type_par(),
                                  name_maps)) {
            cerr << "Error at " << node->getPos()->print() << ": Return type mismatch" << endl;
            cerr << "Expected: " << type_kind_string(current_return_type) << endl;
            cerr << "Got: " << type_kind_string(exp_sem->get_type()) << endl;
            exit(EXIT_FAILURE);
        }

        semant_map->setSemant(node, new AST_Semant(
            AST_Semant::Kind::Value,
            current_return_type,
            exp_sem->get_type_par(),  // 使用方法声明的返回类型参数，实现向上转型
            false  // Return value is not an lvalue
        ));
    } else {
        if (current_return_type != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": Missing return value for non-void method" << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void AST_Semant_Visitor::visit(Continue* node) {
    if (!in_loop) {
        cerr << "Error at " << node->getPos()->print() << ": Continue statement outside loop" << endl;
        exit(EXIT_FAILURE);
    }
}

void AST_Semant_Visitor::visit(Break* node) {
    if (!in_loop) {
        cerr << "Error at " << node->getPos()->print() << ": Break statement outside loop" << endl;
        exit(EXIT_FAILURE);
    }
}

void AST_Semant_Visitor::visit(ArrayExp* node) {
    if (node == nullptr) return;
    
    node->arr->accept(*this);
    node->index->accept(*this);
    
    auto arr_sem = semant_map->getSemant(node->arr);
    auto idx_sem = semant_map->getSemant(node->index);
    
    // Check array type
    if (arr_sem->get_type() != TypeKind::ARRAY) {
        cerr << "Error at " << node->getPos()->print() << ": Array access on non-array type" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Check index type
    if (idx_sem->get_type() != TypeKind::INT) {
        cerr << "Error at " << node->getPos()->print() << ": Array index must be integer" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Set semantic info for array access
    semant_map->setSemant(node, new AST_Semant(
        AST_Semant::Kind::Value,
        TypeKind::INT,
        monostate(),
        true  // Array element is an lvalue
    ));
}

void AST_Semant_Visitor::visit(Type* node) {
    if (node == nullptr) return;
    // Type node itself doesn't need semantic info
}

void AST_Semant_Visitor::visit(VarDecl* node) {
    DEBUG_PRINT("\n=== Visiting VarDecl ===");
    if (node == nullptr) return;
    
    if (node->type != nullptr) {
        node->type->accept(*this);
        DEBUG_PRINT("Variable type: " << static_cast<int>(node->type->typeKind));
    }
    return;
}

void AST_Semant_Visitor::visit(Formal* node) {
    if (node == nullptr) return;
    
    if (node->type != nullptr) {
        node->type->accept(*this);
    }
    
    // Set semantic info for formal parameter with array support
    variant<monostate,string,int> type_par;
    if (node->type->typeKind == TypeKind::ARRAY) {
        type_par = node->type->arity ? variant<monostate,string,int>(node->type->arity->val) : variant<monostate,string,int>(0);
    } else if (node->type->typeKind == TypeKind::CLASS) {
        type_par = node->type->cid ? variant<monostate,string,int>(node->type->cid->id) : monostate();
    } else {
        type_par = monostate();
    }
    return;
}

void AST_Semant_Visitor::visit(Nested* node) {
    if (node == nullptr) return;
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            if (s != nullptr) s->accept(*this);
        }
    }
}

void AST_Semant_Visitor::visit(If* node) {
    if (node == nullptr) return;
    
    // Check condition
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto cond_sem = semant_map->getSemant(node->exp);
        if (cond_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": If condition must be boolean" << endl;
            exit(EXIT_FAILURE);
        }
    }
    
    // Process then and else branches
    if (node->stm1 != nullptr) node->stm1->accept(*this);
    if (node->stm2 != nullptr) node->stm2->accept(*this);
}

void AST_Semant_Visitor::visit(PutInt* node) {
    if (node == nullptr) return;
    
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto exp_sem = semant_map->getSemant(node->exp);
        if (exp_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": PutInt argument must be integer" << endl;
        }
    }
}

void AST_Semant_Visitor::visit(PutCh* node) {
    if (node == nullptr) return;
    
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto exp_sem = semant_map->getSemant(node->exp);
        if (exp_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": PutCh argument must be integer" << endl;
        }
    }
}

void AST_Semant_Visitor::visit(PutArray* node) {
    if (node == nullptr) return;
    
    // Check array argument
    if (node->arr != nullptr) {
        node->arr->accept(*this);
        auto arr_sem = semant_map->getSemant(node->arr);
        if (arr_sem->get_type() != TypeKind::ARRAY) {
            cerr << "Error at " << node->getPos()->print() << ": PutArray first argument must be array" << endl;
        }
    }
    
    // Check size argument
    if (node->n != nullptr) {
        node->n->accept(*this);
        auto n_sem = semant_map->getSemant(node->n);
        if (n_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": PutArray size must be integer" << endl;
        }
    }
}

void AST_Semant_Visitor::visit(Starttime* node) {
    // No semantic checking needed
}

void AST_Semant_Visitor::visit(Stoptime* node) {
    // No semantic checking needed
}

void AST_Semant_Visitor::visit(BinaryOp* node) {
    DEBUG_PRINT("\n=== Visiting BinaryOp: " << (node ? node->op->op : "null") << " ===");
    if (node == nullptr) return;
    
    // Process operands
    if (node->left != nullptr) node->left->accept(*this);
    if (node->right != nullptr) node->right->accept(*this);
    
    auto left_sem = semant_map->getSemant(node->left);
    auto right_sem = semant_map->getSemant(node->right);
    
    if (left_sem->get_type() == TypeKind::ARRAY && right_sem->get_type() == TypeKind::ARRAY) {
        
        int left_arity = holds_alternative<int>(left_sem->get_type_par()) ? 
                        get<int>(left_sem->get_type_par()) : 0;
        int right_arity = holds_alternative<int>(right_sem->get_type_par()) ? 
                         get<int>(right_sem->get_type_par()) : 0;
        
        // if (left_arity != right_arity) {
        //     cerr << "Error at " << node->getPos()->print() 
        //          << ": Array operands must have same arity for element-wise operation. "
        //          << "Left arity: " << left_arity << ", Right arity: " << right_arity << endl;
        //     exit(EXIT_FAILURE);
        // }
        
        semant_map->setSemant(node, new AST_Semant(
            AST_Semant::Kind::Value,
            TypeKind::ARRAY,
            left_arity,
            false
        ));
        return;
    }
    
    if ((left_sem->get_type() == TypeKind::ARRAY) != (right_sem->get_type() == TypeKind::ARRAY)) {
        cerr << "Error at " << node->getPos()->print() 
             << ": Cannot perform binary operation between array and non-array types" << endl;
        exit(EXIT_FAILURE);
    }

    // Check operator types
    if (node->op->op == "+" || node->op->op == "-" || 
        node->op->op == "*" || node->op->op == "/") {
        // Arithmetic operators require integer operands
        if (left_sem->get_type() != TypeKind::INT || right_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": Arithmetic operator requires integer operands" << endl;
        }
        semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
    }
    else if (node->op->op == "<" || node->op->op == "<=" ||
             node->op->op == ">" || node->op->op == ">=") {
        // Comparison operators require integer operands and produce boolean
        if (left_sem->get_type() != TypeKind::INT || right_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": Comparison operator requires integer operands" << endl;
        }
        semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
    }
    else if (node->op->op == "==" || node->op->op == "!=") {
        // Equality operators check type compatibility
        if (!check_compatible_types(left_sem->get_type(), left_sem->get_type_par(),
                                  right_sem->get_type(), right_sem->get_type_par(),
                                  name_maps)) {
            cerr << "Error at " << node->getPos()->print() << ": Incompatible types in equality comparison" << endl;
        }
        semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
    }
    else if (node->op->op == "&&" || node->op->op == "||") {
        // Logical operators require boolean operands and produce boolean
        if (left_sem->get_type() != TypeKind::INT || right_sem->get_type() != TypeKind::INT) {
            cerr << "Error at " << node->getPos()->print() << ": Logical operator requires boolean operands" << endl;
        }
        semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
    }
    DEBUG_PRINT("BinaryOp done");
}

void AST_Semant_Visitor::visit(UnaryOp* node) {
    if (node == nullptr) return;
    
    if (node->exp != nullptr) node->exp->accept(*this);
    auto exp_sem = semant_map->getSemant(node->exp);
    
    if (node->op->op == "-") {
        // Negation requires integer operand
        if (exp_sem->get_type() != TypeKind::INT && exp_sem->get_type() != TypeKind::ARRAY) {
            cerr << "Error at " << node->getPos()->print() << ": Negation requires integer operand" << endl;
        }
        semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, exp_sem->get_type(), monostate(), false));
    }
    else if (node->op->op == "!") {
        // Logical not requires boolean operand
        if (exp_sem->get_type() != TypeKind::INT && exp_sem->get_type() != TypeKind::ARRAY) {
            cerr << "Error at " << node->getPos()->print() << ": Logical not requires boolean operand" << endl;
        }
        semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, exp_sem->get_type(), monostate(), false));
    }
}

void AST_Semant_Visitor::visit(CallExp* node) {
    DEBUG_PRINT("\n=== Visiting CallExp ===");
    if (node == nullptr) return;
    
    node->obj->accept(*this);
    auto obj_sem = semant_map->getSemant(node->obj);
    
    if (obj_sem == nullptr || obj_sem->get_type() != TypeKind::CLASS) {
        cerr << "Error at " << node->getPos()->print() 
             << ": Method call on non-class type" << endl;
        exit(EXIT_FAILURE);
    }
    
    string class_name = get<string>(obj_sem->get_type_par());
    string method_name = node->name->id;
    
    // 在当前类及其所有父类中查找方法
    string method_class = class_name;
    bool found_method = false;
    
    // 首先检查当前类
    if (name_maps->is_method(class_name, method_name)) {
        found_method = true;
        method_class = class_name;
    } else {
        // 检查所有父类
        auto ancestors = *name_maps->get_ancestors(class_name);
        for (const auto& ancestor : ancestors) {
            if (name_maps->is_method(ancestor, method_name)) {
                found_method = true;
                method_class = ancestor;
                break;
            }
        }
    }
    
    if (!found_method) {
        cerr << "Error at " << node->getPos()->print() 
             << ": Method '" << method_name 
             << "' not found in class '" << class_name 
             << "' or its ancestors" << endl;
        exit(EXIT_FAILURE);
    }
    
    auto formal_list = name_maps->get_method_formal_list(method_class, method_name);
    if (formal_list == nullptr) {
        cerr << "Error: Could not get formal parameter list for method " << method_name << endl;
        exit(EXIT_FAILURE);
    }

    try {
        string class_name = get<string>(obj_sem->get_type_par());
        DEBUG_PRINT("Method call on class: " << class_name);
        DEBUG_PRINT("Method name: " << node->name->id);
        
        // Check parameters
        auto formal_list = name_maps->get_method_formal_list(method_class, node->name->id);
        if (formal_list == nullptr) {
            cerr << "Error: Could not get formal parameter list for method " << node->name->id << endl;
            exit(EXIT_FAILURE);
        }
        
        size_t param_count = (node->par != nullptr) ? node->par->size() : 0;
        if (formal_list->size() - 1 != param_count) { // -1 for return type
            cerr << "Error at " << node->getPos()->print() 
                 << ": Wrong number of parameters. Expected " << formal_list->size() - 1 
                 << ", got " << param_count << endl;
            exit(EXIT_FAILURE);
        }
        
        if (node->par != nullptr) {
            for (size_t i = 0; i < node->par->size(); i++) {
                auto param = (*node->par)[i];
                if (param == nullptr) continue;
                
                param->accept(*this);
                auto param_sem = semant_map->getSemant(param);
                if (param_sem == nullptr) {
                    cerr << "Error: Could not get semantic info for parameter " << i << endl;
                    continue;
                }
                
                auto formal = (*formal_list)[i];
                if (formal == nullptr || formal->type == nullptr) {
                    cerr << "Error: Invalid formal parameter at index " << i << endl;
                    continue;
                }
                
                variant<monostate,string,int> formal_type_par = 
                    formal->type->cid ? variant<monostate,string,int>(formal->type->cid->id) : monostate();
                
                if (!check_compatible_types(formal->type->typeKind, formal_type_par,
                                         param_sem->get_type(), param_sem->get_type_par(),
                                         name_maps)) {
                    cerr << "Error at " << node->getPos()->print() 
                         << ": Parameter type mismatch at position " << i << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
        
    } catch (const std::bad_variant_access& e) {
        cerr << "Error: Invalid type information in method call" << endl;
    }

    Formal* return_formal = (*formal_list)[formal_list->size() - 1];
    Type* method_type = return_formal->type;
    
    auto method_semant = new AST_Semant(
        AST_Semant::Kind::Value,
        method_type->typeKind,
        method_type->cid ? variant<monostate,string,int>(method_type->cid->id) : monostate(),
        false
    );
    semant_map->setSemant(node, method_semant);
    
    // ...existing code...
}

void AST_Semant_Visitor::visit(ClassVar* node) {
    DEBUG_PRINT("\n=== Visiting ClassVar: " << (node ? node->id->id : "null"));
    if (node == nullptr) return;
    
    // Visit object
    if (node->obj != nullptr) node->obj->accept(*this);
    auto obj_sem = semant_map->getSemant(node->obj);
    
    // Check if accessing a class type
    if (obj_sem->get_type() != TypeKind::CLASS) {
        cerr << "Error at " << node->getPos()->print() << ": Class variable access on non-class type" << endl;
        exit(EXIT_FAILURE);
    }
    
    string class_name = get<string>(obj_sem->get_type_par());
    
    // Check variable exists in class hierarchy
    VarDecl* var_decl = nullptr;
    string current_check_class = class_name;
    var_decl = name_maps->get_class_var(class_name, node->id->id);

    auto ancestors = name_maps->get_ancestors(class_name);
    for (const auto& ancestor : *ancestors) {
        if (var_decl != nullptr) {
            break;
        }
        var_decl = name_maps->get_class_var(ancestor, node->id->id);
    }
    if (var_decl == nullptr) {
        cerr << "Error at " << node->getPos()->print() << ": Variable " << node->id->id 
             << " not found in class " << class_name << " or its ancestors" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Set semantic info for class variable with array support
    variant<monostate,string,int> type_par;
    if (var_decl->type->typeKind == TypeKind::ARRAY) {
        type_par = var_decl->type->arity ? variant<monostate,string,int>(var_decl->type->arity->val) : variant<monostate,string,int>(0);
    } else if (var_decl->type->typeKind == TypeKind::CLASS) {
        type_par = var_decl->type->cid ? variant<monostate,string,int>(var_decl->type->cid->id) : monostate();
    } else {
        type_par = monostate();
    }
    
    semant_map->setSemant(node, new AST_Semant(
        AST_Semant::Kind::Value,
        var_decl->type->typeKind,
        type_par,
        true  // Class variables are lvalues
    ));
}

void AST_Semant_Visitor::visit(BoolExp* node) {
    if (node == nullptr) return;
    semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
}

void AST_Semant_Visitor::visit(This* node) {
    if (node == nullptr) return;
    
    // Check if this is used inside a method
    if (current_method.empty()) {
        cerr << "Error at " << node->getPos()->print() << ": 'this' used outside method context" << endl;
        return;
    }
    
    semant_map->setSemant(node, new AST_Semant(
        AST_Semant::Kind::Value,
        TypeKind::CLASS,
        current_class,
        false
    ));
}

void AST_Semant_Visitor::visit(Length* node) {
    if (node == nullptr) return;
    
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto exp_sem = semant_map->getSemant(node->exp);
        
        if (exp_sem->get_type() != TypeKind::ARRAY) {
            cerr << "Error at " << node->getPos()->print() << ": Length operator requires array operand" << endl;
        }
    }
    
    semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
}

void AST_Semant_Visitor::visit(Esc* node) {
    if (node == nullptr) return;
    
    // Process statements
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            if (s != nullptr) s->accept(*this);
        }
    }
    
    // Process expression
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto exp_sem = semant_map->getSemant(node->exp);
        semant_map->setSemant(node, exp_sem);  // Propagate expression's semantic info
    }
}

void AST_Semant_Visitor::visit(GetInt* node) {
    if (node == nullptr) return;
    semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
}

void AST_Semant_Visitor::visit(GetCh* node) {
    if (node == nullptr) return;
    semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
}

void AST_Semant_Visitor::visit(GetArray* node) {
    if (node == nullptr) return;
    
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        auto exp_sem = semant_map->getSemant(node->exp);
        // if(exp_sem->get_type() == TypeKind::ARRAY)
        //     cerr << "exp_sem->get_type() == TypeKind::ARRAY" << endl;
        if (exp_sem->get_type() != TypeKind::ARRAY) {
            cerr << "Error at " << node->getPos()->print() << ": GetArray size must be integer" << endl;
        }
    }
    
    semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, 0, false));
}

void AST_Semant_Visitor::visit(IdExp* node) {
    DEBUG_PRINT("\n=== Visiting IdExp: " << (node ? node->id : "null") << " ===");
    DEBUG_PRINT("current_class: " << current_class << " current_method: " << current_method);
    if (node == nullptr) return;
    
    for (const auto& class_name : *name_maps->get_all_classes()) {
        if (name_maps->is_method(class_name, node->id)) {
            DEBUG_PRINT("Found method: " << node->id << " in class " << class_name);
            auto formal_list = name_maps->get_method_formal_list(class_name, node->id);
            if (formal_list != nullptr && !formal_list->empty()) {
                Formal* return_formal = (*formal_list)[formal_list->size() - 1];
                Type* method_type = return_formal->type;
                
                variant<monostate,string,int> type_par;
                if (method_type->typeKind == TypeKind::ARRAY) {
                    type_par = method_type->arity ? variant<monostate,string,int>(method_type->arity->val) : variant<monostate,string,int>(0);
                } else if (method_type->typeKind == TypeKind::CLASS) {
                    type_par = method_type->cid ? variant<monostate,string,int>(method_type->cid->id) : monostate();
                } else {
                    type_par = monostate();
                }
                
                semant_map->setSemant(node, new AST_Semant(
                    AST_Semant::Kind::MethodName,
                    method_type->typeKind,
                    type_par,
                    false
                ));
                return;
            }
        }
    }
    
    if (!current_method.empty()) {
        auto method_var = name_maps->get_method_var(current_class, current_method, node->id);
        DEBUG_PRINT("Method var"<< " current class: "<<current_class<< " current method: "<<current_method<< " node id: "<<node->id);
        if (method_var != nullptr) {
            variant<monostate,string,int> type_par;
            if (method_var->type->typeKind == TypeKind::ARRAY) {
                DEBUG_PRINT("ARRAY");
                type_par = method_var->type->arity ? variant<monostate,string,int>(method_var->type->arity->val) : variant<monostate,string,int>(0);
            } else if (method_var->type->typeKind == TypeKind::CLASS) {
                DEBUG_PRINT("CLASS: " << method_var->type->cid->id);
                type_par = method_var->type->cid ? variant<monostate,string,int>(method_var->type->cid->id) : monostate();
            } else {
                type_par = monostate();
            }
            semant_map->setSemant(node, new AST_Semant(
                AST_Semant::Kind::Value,
                method_var->type->typeKind,
                type_par,
                true
            ));
            return;
        }
        
        // Check method parameters
        auto formal = name_maps->get_method_formal(current_class, current_method, node->id);
        if (formal != nullptr) {
            variant<monostate,string,int> type_par;
            if (formal->type->typeKind == TypeKind::ARRAY) {
                type_par = formal->type->arity ? variant<monostate,string,int>(formal->type->arity->val) : variant<monostate,string,int>(0);
            } else if (formal->type->typeKind == TypeKind::CLASS) {
                type_par = formal->type->cid ? variant<monostate,string,int>(formal->type->cid->id) : monostate();
            } else {
                type_par = monostate();
            }
            
            semant_map->setSemant(node, new AST_Semant(
                AST_Semant::Kind::Value,
                formal->type->typeKind,
                type_par,
                true
            ));
            DEBUG_PRINT("IdExp semantic info set with type " << static_cast<int>(formal->type->typeKind));
            return;
        }
    }
    
    // Check class fields
    auto class_var = name_maps->get_class_var(current_class, node->id);
    DEBUG_PRINT("Class var"<< " current class: "<<current_class<< " node id: "<<node->id);
    if (class_var != nullptr) {
        variant<monostate,string,int> type_par;
        if (class_var->type->typeKind == TypeKind::ARRAY) {
            type_par = class_var->type->arity ? variant<monostate,string,int>(class_var->type->arity->val) : variant<monostate,string,int>(0);
        } else if (class_var->type->typeKind == TypeKind::CLASS) {
            type_par = class_var->type->cid ? variant<monostate,string,int>(class_var->type->cid->id) : monostate();
        } else {
            type_par = monostate();
        }
        
        semant_map->setSemant(node, new AST_Semant(
            AST_Semant::Kind::Value,
            class_var->type->typeKind,
            type_par,
            true
        ));
        return;
    }
    
    cerr << "Error at " << node->getPos()->print() << ": Undefined variable " << node->id << endl;
}

void AST_Semant_Visitor::visit(OpExp* node) {
    if (node == nullptr) return;
    // Operator expressions don't need semantic info
}

void AST_Semant_Visitor::visit(IntExp* node) {
    DEBUG_PRINT("\n=== Visiting IntExp: " << (node ? node->val : -1) << " ===");
    if (node == nullptr) return;
    semant_map->setSemant(node, new AST_Semant(AST_Semant::Kind::Value, TypeKind::INT, monostate(), false));
}
