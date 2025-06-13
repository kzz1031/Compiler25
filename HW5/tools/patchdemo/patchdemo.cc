#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "temp.hh"
#include "treep.hh"
#include "tr_exp.hh"
#include "tree2xml.hh"

using namespace std;
using namespace tree;
using namespace tinyxml2;

tree::Program* generate_a_testIR(Temp_map *tm); //forward declaration

int main(int argc, const char *argv[]) {
    string file;

    const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;

    if ((!debug && argc != 2) || (debug && argc != 3)) {
        cerr << "Usage: " << argv[0] << " [--debug] filename" << endl;
        return EXIT_FAILURE;
    }
    file = argv[argc - 1];

    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_ast = file + ".2-semant.ast"; // ast in xml
    string file_irp = file + ".3.irp";
    string file_irp_canon = file + ".3-canon.irp";

    Temp_map *tm = new Temp_map();
    tree::Program* ir = generate_a_testIR(tm);
    cout << "-----Save to XML---" << endl;
    XMLDocument *x = tree2xml(ir);
    x->SaveFile(file_irp.c_str());
    std::cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}

/*
public int main ()  {
    if ( 1>2 || 2>1 )
        return 100;
    else
        return 200;
    x = 3>4;
    while (x) {
        x = 0; 
        continue;
        putchar(({x=2;} x));
        break;
    }
    return 0;
}
*/
tree::Program* generate_a_testIR(Temp_map *tm) {
    tree::Label *entry_label = tm->newlabel();

    //* now construct the Tr_cx for 1>2
    tree::Label *label_1_2_true = tm->newlabel();
    tree::Label *label_1_2_false = tm->newlabel();
    Patch_list* t_1_2_true_list = new Patch_list(); t_1_2_true_list->add_patch(label_1_2_true);
    Patch_list* t_1_2_false_list = new Patch_list(); t_1_2_false_list->add_patch(label_1_2_false);
    Tr_cx *t_1_2 = new Tr_cx(t_1_2_true_list, t_1_2_false_list, 
                   new tree::Cjump(">", new tree::Const(1), new tree::Const(2), label_1_2_true, label_1_2_false));

    //* now construct the Tr_cx for 2>1
    tree::Label *label_2_1_true = tm->newlabel();
    tree::Label *label_2_1_false = tm->newlabel();
    Patch_list* t_2_1_true_list = new Patch_list(); t_2_1_true_list->add_patch(label_2_1_true);
    Patch_list* t_2_1_false_list = new Patch_list(); t_2_1_false_list->add_patch(label_2_1_false);
    Tr_cx *t_2_1 = new Tr_cx(t_2_1_true_list, t_2_1_false_list,
                   new tree::Cjump(">", new tree::Const(2), new tree::Const(1), label_2_1_true, label_2_1_false));

    //* now construct the Tr_cx for 1>2 || 2>1
    vector<Stm*> *sl = new vector<Stm*>();
    sl->push_back(new tree::LabelStm(entry_label));
    
    vector<Stm*> *if_cond_sl = new vector<Stm*>();
    if_cond_sl->push_back(t_1_2->stm);
    Label *label_middle = tm->newlabel();
    if_cond_sl->push_back(new LabelStm(label_middle));
    t_1_2_false_list->patch(label_middle); //patch the false list of t_1_2 to label_middle
    if_cond_sl->push_back(t_2_1->stm);
    t_1_2_true_list->add(t_2_1_true_list);
    Tr_cx *if_cond_cx = new Tr_cx(t_1_2_true_list, t_2_1_false_list, new tree::Seq(if_cond_sl));

    //* now construct the if statment
    tree::Label *label_if_true = tm->newlabel();
    tree::Label *label_if_false = tm->newlabel();
    tree::Label *label_if_end = tm->newlabel();
    if_cond_cx->true_list->patch(label_if_true); //patch the true list to label_if_true
    if_cond_cx->false_list->patch(label_if_false); //patch the true list to label_if_false

    sl->push_back(if_cond_cx->stm);
    sl->push_back(new LabelStm(label_if_true));
    sl->push_back(new Return(new tree::Const(100))); //return 100
    sl->push_back(new Jump(label_if_end));
    sl->push_back(new LabelStm(label_if_false));
    sl->push_back(new Return(new tree::Const(200))); //return 200
    sl->push_back(new LabelStm(label_if_end));

    //now construct the x = 3>4
    //now construct the 3>4
    tree::Label *label_x_3_4_true = tm->newlabel();
    tree::Label *label_x_3_4_false = tm->newlabel();
    Patch_list *t_x_3_4_true_list = new Patch_list(); t_x_3_4_true_list->add_patch(label_x_3_4_true);
    Patch_list *t_x_3_4_false_list = new Patch_list(); t_x_3_4_false_list->add_patch(label_x_3_4_false);
    Tr_cx *t_x_3_4 = new Tr_cx(t_x_3_4_true_list, t_x_3_4_false_list,
                   new tree::Cjump(">", new tree::Const(3), new tree::Const(4), label_x_3_4_true, label_x_3_4_false));
    
    TempExp *var_x = new TempExp(tree::Type::INT, tm->newtemp());
    tree::Exp *right = t_x_3_4->unEx(tm)->exp;
    //now the assignmet
    sl->push_back(new tree::Move(var_x, right));
    
    //prepare for the while loop
    tree::Label *label_while = tm->newlabel();
    tree::Label *label_while_true = tm->newlabel();
    tree::Label *label_while_false = tm->newlabel();

    //prepare the external call
    vector<tree::Stm*> *sl1 = new vector<tree::Stm*>();
    sl1->push_back(new tree::Move(var_x, new tree::Const(2)));
    tree::Eseq *eseq = new tree::Eseq(tree::Type::INT, new tree::Seq(sl1), var_x);
    vector<tree::Exp*> *args = new vector<tree::Exp*>();
    args->push_back(eseq);
    tree::ExtCall *call = new tree::ExtCall(tree::Type::INT, "putchar", args);

    //now the while loop
    Tr_cx *t_while_cx = (new Tr_ex(var_x))->unCx(tm);
    t_while_cx->true_list->patch(label_while_true);
    t_while_cx->false_list->patch(label_while_false);
    sl->push_back(new tree::LabelStm(label_while));
    sl->push_back(t_while_cx->stm);
    sl->push_back(new tree::LabelStm(label_while_true));
    sl->push_back(new tree::Move(var_x, new tree::Const(0)));
    sl->push_back(new tree::Jump(label_while)); //this is for the continue
    sl->push_back(new tree::ExpStm(call));
    sl->push_back(new tree::Jump(label_while_false)); //this is for the break
    sl->push_back(new tree::Jump(label_while));
    sl->push_back(new tree::LabelStm(label_while_false));

    //now construct the return statement
    sl->push_back(new tree::Return(new tree::Const(0)));
    return new tree::Program(new vector<tree::FuncDecl*>(1,
            new tree::FuncDecl("_^main^_main", nullptr, 
                new vector<tree::Block*>(1, new tree::Block(entry_label, nullptr, sl)), 
                tree::Type::INT, tm->next_temp, tm->next_label)));
}