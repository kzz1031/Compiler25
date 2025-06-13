#define DEBUG
#undef DEBUG

#include <vector>
#include <map>
#include <string>
#include "quad.hh"
#include "blocking.hh"
#include "temp.hh"

using namespace std;
using namespace quad;

static QuadFuncDecl* blocking(QuadFuncDecl* func_decl);
static vector<QuadBlock*>* split_block(QuadBlock* block, Temp_map * temp_map);

QuadProgram *blocking(QuadProgram *program) {
    if (program == nullptr || program->quadFuncDeclList == nullptr || program->quadFuncDeclList->empty()) {
        return program;  // Nothing to do
    }

#ifdef DEBUG
    cout << "Blocking the quad" << endl;
#endif
    
    vector<QuadFuncDecl*>* new_func_decl_list = new vector<QuadFuncDecl*>();
    
    // Process each function declaration in the program
    for (auto func_decl : *(program->quadFuncDeclList)) {
#ifdef DEBUG
        cout << "Blocking function: " << func_decl->funcname << endl;
#endif
        QuadFuncDecl* new_func_decl = blocking(func_decl);
        if (new_func_decl != nullptr) {
            new_func_decl_list->push_back(new_func_decl);
        }
    }
#ifdef DEBUG
    cout << "Finished blocking functions" << endl;
#endif
    
    // Create a new program with the modified function declarations
    QuadProgram* new_program = new QuadProgram(
        static_cast<tree::Program*>(program->node),
        new_func_decl_list
    );
    
    return new_program;
}

static QuadFuncDecl* blocking(QuadFuncDecl* func_decl) {
    if (func_decl == nullptr || func_decl->quadblocklist == nullptr || func_decl->quadblocklist->empty()) {
        return func_decl;  // Nothing to do
    }
#ifdef DEBUG
    cout << "Now blocking function: " << func_decl->funcname << endl;
#endif
    
    Temp_map * temp_map = new Temp_map();
    temp_map->next_label = func_decl->last_label_num + 1;
    temp_map->next_temp = func_decl->last_temp_num + 1;

#ifdef DEBUG
    cout << "Next label: " << temp_map->next_label << endl;
    cout << "Next temp: " << temp_map->next_temp << endl;
#endif

    vector<QuadBlock*>* new_blocks = new vector<QuadBlock*>();
    
    // Process each block in the function
    for (auto block : *func_decl->quadblocklist) {
        if (block == nullptr || block->quadlist == nullptr || block->quadlist->empty()) {
            // Keep empty blocks as is
            new_blocks->push_back(block);
            continue;
        }
        
        // Process each block by splitting it into smaller blocks based on control flow
        vector<QuadBlock*>* split_blocks = split_block(block, temp_map);
        if (split_blocks != nullptr) {
            new_blocks->insert(new_blocks->end(), split_blocks->begin(), split_blocks->end());
            delete split_blocks;
        }
    }
    
    // Create a new function declaration with the new blocks
    QuadFuncDecl* new_func_decl = new QuadFuncDecl(
        func_decl->node,
        func_decl->funcname,
        func_decl->params,
        new_blocks,
        func_decl->last_label_num,
        func_decl->last_temp_num
    );
    
    return new_func_decl;
}

static vector<QuadBlock*>* split_block(QuadBlock* block, Temp_map * temp_map) {
    if (block == nullptr || block->quadlist == nullptr || block->quadlist->empty()) {
        return nullptr;
    }
#ifdef DEBUG
    cout << "Splitting block: " << block->entry_label->num << endl;
#endif
    
    vector<QuadBlock*>* result_blocks = new vector<QuadBlock*>();
    vector<QuadStm*>* current_stmts = new vector<QuadStm*>();
    Label* current_label = block->entry_label;
    
    // Map to keep track of labels we've seen
    map<string, int> label_seen;
    
    // Process statements in sequence
    for (size_t i = 0; i < block->quadlist->size(); ++i) {
        QuadStm* stmt = block->quadlist->at(i);
        if (stmt == nullptr) continue;
#ifdef DEBUG
        cout << "Processing statement: " << quadKindToString(stmt->kind) << endl;
#endif
        
        // Check if this statement is a label
        if (stmt->kind == QuadKind::LABEL) {
            QuadLabel* label_stmt = dynamic_cast<QuadLabel*>(stmt);
            if (label_stmt && label_stmt->label) {
                // If we've seen statements already, end the current block 
                // and start a new one with this label
                if (!current_stmts->empty()) {
                    // Add a jump to this label
                    set<Temp*>* empty_def = new set<Temp*>();
                    set<Temp*>* empty_use = new set<Temp*>();
                    QuadJump* jump = new QuadJump(nullptr, label_stmt->label, empty_def, empty_use);
                    current_stmts->push_back(jump);
                    
                    // Create exit label vector
                    vector<Label*>* exit_labels = new vector<Label*>();
                    exit_labels->push_back(label_stmt->label);
                    
                    // Create a new block and add it to the result
                    QuadBlock* new_block = new QuadBlock(nullptr, current_stmts, current_label, exit_labels);
                    result_blocks->push_back(new_block);
                    
                    // Start a new block
                    current_stmts = new vector<QuadStm*>();
                    current_label = label_stmt->label;
                }
                // Add the label statement to the new block
                current_stmts->push_back(stmt);
                continue;  // Continue to next statement
            }
        }
        
        // Add the current statement to the current block
        current_stmts->push_back(stmt);
        
        // Check for control flow changes that should end the block
        bool end_block = false;
        vector<Label*>* exit_labels = new vector<Label*>();
        
        // Check if this statement is a jump
        if (stmt->kind == QuadKind::JUMP) {
#ifdef DEBUG
            cout << "Found a jump statement" << endl;
#endif
            QuadJump* jump = dynamic_cast<QuadJump*>(stmt);
            if (jump && jump->label) {
                end_block = true;
                exit_labels->push_back(jump->label);
            }
        }
        // Check if this statement is a conditional jump
        else if (stmt->kind == QuadKind::CJUMP) {
#ifdef DEBUG
            cout << "Found a conditional jump statement" << endl;
#endif
            QuadCJump* cjump = dynamic_cast<QuadCJump*>(stmt);
            if (cjump && cjump->t && cjump->f) {
                end_block = true;
                exit_labels->push_back(cjump->t);
                exit_labels->push_back(cjump->f);
            }
        }
        // Check if this statement is a return
        else if (stmt->kind == QuadKind::RETURN) {
#ifdef DEBUG
            cout << "Found a return statement" << endl;
#endif
            end_block = true;
            // Return has no exit labels (it exits the function)
        }
        // Check if this statement is an exit call
        else if (stmt->kind == QuadKind::EXTCALL) {
#ifdef DEBUG
            cout << "Found an external call statement" << endl;
#endif
            QuadExtCall* extcall = dynamic_cast<QuadExtCall*>(stmt);
                // Check if the external call is an exit
            if (extcall->extfun == "exit") {
#ifdef DEBUG
                cout << "Found an exit call statement" << endl;
#endif
                end_block = true;
                // exit() has no exit labels
            }
        } else if (stmt->kind == QuadKind::MOVE_EXTCALL) {
#ifdef DEBUG
            cout << "Found a move external call statement" << endl;
#endif
            QuadMoveExtCall* move_extcall = dynamic_cast<QuadMoveExtCall*>(stmt);
            if (move_extcall && move_extcall->extcall->extfun == "exit") {
#ifdef DEBUG
                cout << "Found an exit call statement" << endl;
#endif
                current_stmts->pop_back();  // Remove the move_extcall from the current block
                move_extcall->extcall->def = new set<Temp*>();  // Clear the def set
                current_stmts->push_back(move_extcall->extcall);  // Add the extcall to the current block 
                //because moving the call result to a temp is not needed
                end_block = true;
                // exit() has no exit labels
            }
        }
        
        if (end_block) {
#ifdef DEBUG
            cout << "Created a new block with entry label: " << current_label->num << endl;
#endif
            // Create a new block and add it to the result
            QuadBlock* new_block = new QuadBlock(nullptr, current_stmts, current_label, exit_labels);
            result_blocks->push_back(new_block);
            
            // Start a new block if there are more statements
            if (i < block->quadlist->size() - 1) {
                current_stmts = new vector<QuadStm*>();
                if (i + 1 < block->quadlist->size()) {
                    // Check if the next statement is a label
                    QuadStm* next_stmt = block->quadlist->at(i + 1);
                    if (next_stmt && next_stmt->kind == QuadKind::LABEL) {
                        QuadLabel* next_label = dynamic_cast<QuadLabel*>(next_stmt);
                        current_label = next_label->label;  // Use the label from the next statement
                    } else
                        current_label = temp_map->newlabel();  // Generate a new label
                }
            } else 
                current_stmts = new vector<QuadStm*>();  // No more statements to process

        }
    }
    
    // If we have remaining statements that didn't end with a control flow change,
    // create a final block
    if (!current_stmts->empty()) {
        vector<Label*>* exit_labels = nullptr;
        if (block->exit_labels != nullptr && !block->exit_labels->empty()) {
            exit_labels = new vector<Label*>(*block->exit_labels);  // Copy exit labels from original block
        } else {
            exit_labels = new vector<Label*>();
        }
        
#ifdef DEBUG
        cout << "Creating final block with entry label: " << current_label->num << endl;
#endif
        QuadBlock* new_block = new QuadBlock(nullptr, current_stmts, current_label, exit_labels);
        result_blocks->push_back(new_block);
    }
    
    return result_blocks;
}