#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "quad.hh"
#include "temp.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace quad;
using namespace tinyxml2;

// Helper function declarations
static QuadProgram* parseProgram(XMLElement* element);
static QuadFuncDecl* parseFuncDecl(XMLElement* element);
static QuadBlock* parseBlock(XMLElement* element);
static QuadStm* parseStatement(XMLElement* element);
static QuadTerm* parseQuadTerm(XMLElement* element);
static TempExp* parseTempExp(const char* tempName, const char* typeStr = nullptr);
static set<Temp*>* parseDefUse(XMLElement* element, const char* defOrUse);
static Label* parseLabel(const char* labelStr);


static tree::Type stringToType(const string& typeStr) {
    #ifdef DEBUG
        cout << "Converting string to type: " << typeStr << endl;
    #endif
        if (string(typeStr) == "INT") return tree::Type::INT;
        if (string(typeStr) == "PTR") return tree::Type::PTR;
        throw runtime_error("Unknown type: " + typeStr);
    }

// Global temp and label maps to ensure we reuse the same objects
static map<string, Temp*> tempMap;
static map<string, Label*> labelMap;

// Convert XML to Quad program
QuadProgram* xml2quad(const char* filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        cerr << "Failed to load XML file: " << filename << endl;
        return nullptr;
    }

    XMLElement* root = doc.RootElement();
    if (!root || strcmp(root->Name(), "program") != 0) {
        cerr << "Invalid XML format: missing root program element" << endl;
        return nullptr;
    }

    return parseProgram(root);
}

// Parse a program element (root of the XML document)
static QuadProgram* parseProgram(XMLElement* element) {
    vector<QuadFuncDecl*>* funcList = new vector<QuadFuncDecl*>();
    
    // Iterate through all function declarations
    for (XMLElement* funcElem = element->FirstChildElement("funcdecl"); 
         funcElem; 
         funcElem = funcElem->NextSiblingElement("funcdecl")) {
        QuadFuncDecl* func = parseFuncDecl(funcElem);
        if (func) {
            funcList->push_back(func);
        }
    }
    
    return new QuadProgram(nullptr, funcList);
}

// Parse a function declaration
static QuadFuncDecl* parseFuncDecl(XMLElement* element) {
    const char* funcName = element->Attribute("name");
    if (!funcName) {
        cerr << "Function declaration missing name attribute" << endl;
        return nullptr;
    }
    
    // Read last_label_num and last_temp_num attributes
    int last_label_num = 0;
    int last_temp_num = 0;
    element->QueryIntAttribute("last_label_num", &last_label_num);
    element->QueryIntAttribute("last_temp_num", &last_temp_num);
    
    // Parse parameters
    vector<Temp*>* params = new vector<Temp*>();
    XMLElement* paramsElem = element->FirstChildElement("params");
    if (paramsElem) {
        for (XMLElement* paramElem = paramsElem->FirstChildElement("param"); 
             paramElem; 
             paramElem = paramElem->NextSiblingElement("param")) {
            const char* tempName = paramElem->Attribute("temp");
            if (tempName) {
                Temp* temp = new Temp(stoi(tempName)); 
                if (temp) {
                    params->push_back(temp);
                }
            }
        }
    }
    
    // Parse blocks
    vector<QuadBlock*>* blocks = new vector<QuadBlock*>();
    for (XMLElement* blockElem = element->FirstChildElement("block"); 
         blockElem; 
         blockElem = blockElem->NextSiblingElement("block")) {
        QuadBlock* block = parseBlock(blockElem);
        if (block) {
            blocks->push_back(block);
        }
    }
    
    return new QuadFuncDecl(nullptr, funcName, params, blocks, last_label_num, last_temp_num);
}

// Parse a block
static QuadBlock* parseBlock(XMLElement* element) {
    // Parse entry label
    const char* entryLabelStr = element->Attribute("entry");
    Label* entryLabel = entryLabelStr ? parseLabel(entryLabelStr) : nullptr;
    
    // Parse exit labels
    vector<Label*>* exitLabels = new vector<Label*>();
    XMLElement* exitsElem = element->FirstChildElement("exits");
    if (exitsElem) {
        for (XMLElement* exitElem = exitsElem->FirstChildElement("exit"); 
             exitElem; 
             exitElem = exitElem->NextSiblingElement("exit")) {
            const char* labelStr = exitElem->Attribute("label");
            if (labelStr) {
                Label* label = parseLabel(labelStr);
                if (label) {
                    exitLabels->push_back(label);
                }
            }
        }
    }
    
    // Parse statements
    vector<QuadStm*>* stmts = new vector<QuadStm*>();
    for (XMLElement* stmtElem = element->FirstChildElement(); 
         stmtElem; 
         stmtElem = stmtElem->NextSiblingElement()) {
        // Skip exits element since we already processed it
        if (strcmp(stmtElem->Name(), "exits") == 0) {
            continue;
        }
        
        QuadStm* stmt = parseStatement(stmtElem);
        if (stmt) {
            stmts->push_back(stmt);
        }
    }
    
    return new QuadBlock(nullptr, stmts, entryLabel, exitLabels);
}

// Parse a statement
static QuadStm* parseStatement(XMLElement* element) {
    const char* kind = element->Attribute("kind");
    if (!kind) {
        cerr << "Statement missing kind attribute" << endl;
        return nullptr;
    }
    
    // Parse def/use sets that are common to all statement types
    set<Temp*>* def = parseDefUse(element, "def");
    set<Temp*>* use = parseDefUse(element, "use");
    
    // Parse specific statement types
    if (strcmp(kind, "MOVE") == 0) {
        const char* dstName = element->Attribute("dst");
        const char* dstType = element->Attribute("temp_type");
        TempExp* dst = dstName ? parseTempExp(dstName, dstType) : nullptr;
        
        XMLElement* srcElem = element->FirstChildElement("src");
        QuadTerm* src = srcElem ? parseQuadTerm(srcElem) : nullptr;
        
        return new QuadMove(nullptr, dst, src, def, use);
    } 
    else if (strcmp(kind, "LOAD") == 0) {
        const char* dstName = element->Attribute("dst");
        const char* dstType = element->Attribute("temp_type");
        TempExp* dst = dstName ? parseTempExp(dstName, dstType) : nullptr;
        
        XMLElement* srcElem = element->FirstChildElement("src");
        QuadTerm* src = srcElem ? parseQuadTerm(srcElem) : nullptr;
        
        return new QuadLoad(nullptr, dst, src, def, use);
    } 
    else if (strcmp(kind, "STORE") == 0) {
        XMLElement* srcElem = element->FirstChildElement("src");
        QuadTerm* src = srcElem ? parseQuadTerm(srcElem) : nullptr;
        
        XMLElement* dstElem = element->FirstChildElement("dst");
        QuadTerm* dst = dstElem ? parseQuadTerm(dstElem) : nullptr;
        
        return new QuadStore(nullptr, src, dst, def, use);
    } 
    else if (strcmp(kind, "MOVE_BINOP") == 0) {
        const char* dstName = element->Attribute("dst");
        const char* dstType = element->Attribute("temp_type");
        TempExp* dst = dstName ? parseTempExp(dstName, dstType) : nullptr;
        
        const char* op = element->Attribute("op");
        string binop = op ? op : "";
        
        XMLElement* leftElem = element->FirstChildElement("left");
        QuadTerm* left = leftElem ? parseQuadTerm(leftElem) : nullptr;
        
        XMLElement* rightElem = element->FirstChildElement("right");
        QuadTerm* right = rightElem ? parseQuadTerm(rightElem) : nullptr;
        
        return new QuadMoveBinop(nullptr, dst, left, binop, right, def, use);
    } 
    else if (strcmp(kind, "CALL") == 0) {
        const char* name = element->Attribute("name");
        string funcName = name ? name : "";
        
        const char* resultName = element->Attribute("result");
        const char* resultType = element->Attribute("temp_type");
        TempExp* resultTemp = resultName ? parseTempExp(resultName, resultType) : nullptr;
        
        XMLElement* objElem = element->FirstChildElement("object");
        QuadTerm* objTerm = objElem ? parseQuadTerm(objElem) : nullptr;
        
        vector<QuadTerm*>* args = new vector<QuadTerm*>();
        XMLElement* argsElem = element->FirstChildElement("args");
        if (argsElem) {
            for (XMLElement* argElem = argsElem->FirstChildElement("arg"); 
                 argElem; 
                 argElem = argElem->NextSiblingElement("arg")) {
                XMLElement* valueElem = argElem->FirstChildElement("value");
                if (valueElem) {
                    QuadTerm* arg = parseQuadTerm(valueElem);
                    if (arg) {
                        args->push_back(arg);
                    }
                }
            }
        }
        
        return new QuadCall(nullptr, funcName, objTerm, args, def, use);
    } 
    else if (strcmp(kind, "MOVE_CALL") == 0) {
        const char* dstName = element->Attribute("dst");
        const char* dstType = element->Attribute("temp_type");
        TempExp* dst = dstName ? parseTempExp(dstName, dstType) : nullptr;
        
        // Find the nested call element and parse it
        XMLElement* callElem = element->FirstChildElement("call");
        QuadCall* call = callElem ? static_cast<QuadCall*>(parseStatement(callElem)) : nullptr;
        
        return new QuadMoveCall(nullptr, dst, call, def, use);
    } 
    else if (strcmp(kind, "EXTCALL") == 0) {
        const char* name = element->Attribute("name");
        string extFun = name ? name : "";
        
        const char* resultName = element->Attribute("result");
        const char* resultType = element->Attribute("temp_type");
        TempExp* resultTemp = resultName ? parseTempExp(resultName, resultType) : nullptr;
        
        vector<QuadTerm*>* args = new vector<QuadTerm*>();
        XMLElement* argsElem = element->FirstChildElement("args");
        if (argsElem) {
            for (XMLElement* argElem = argsElem->FirstChildElement("arg"); 
                 argElem; 
                 argElem = argElem->NextSiblingElement("arg")) {
                XMLElement* valueElem = argElem->FirstChildElement("value");
                if (valueElem) {
                    QuadTerm* arg = parseQuadTerm(valueElem);
                    if (arg) {
                        args->push_back(arg);
                    }
                }
            }
        }
        
        return new QuadExtCall(nullptr, extFun, args, def, use);
    } 
    else if (strcmp(kind, "MOVE_EXTCALL") == 0) {
        const char* dstName = element->Attribute("dst");
        const char* dstType = element->Attribute("temp_type");
        TempExp* dst = dstName ? parseTempExp(dstName, dstType) : nullptr;
        
        // Find the nested extcall element and parse it
        XMLElement* extCallElem = element->FirstChildElement("extcall");
        QuadExtCall* extCall = extCallElem ? static_cast<QuadExtCall*>(parseStatement(extCallElem)) : nullptr;
        
        return new QuadMoveExtCall(nullptr, dst, extCall, def, use);
    } 
    else if (strcmp(kind, "LABEL") == 0) {
        const char* labelName = element->Attribute("name");
        Label* label = labelName ? parseLabel(labelName) : nullptr;
        
        return new QuadLabel(nullptr, label, def, use);
    } 
    else if (strcmp(kind, "JUMP") == 0) {
        const char* targetName = element->Attribute("target");
        Label* target = targetName ? parseLabel(targetName) : nullptr;
        
        return new QuadJump(nullptr, target, def, use);
    } 
    else if (strcmp(kind, "CJUMP") == 0) {
        const char* relopStr = element->Attribute("relop");
        string relop = relopStr ? relopStr : "";
        
        XMLElement* leftElem = element->FirstChildElement("left");
        QuadTerm* left = leftElem ? parseQuadTerm(leftElem) : nullptr;
        
        XMLElement* rightElem = element->FirstChildElement("right");
        QuadTerm* right = rightElem ? parseQuadTerm(rightElem) : nullptr;
        
        const char* trueLabel = element->Attribute("true");
        Label* t = trueLabel ? parseLabel(trueLabel) : nullptr;
        
        const char* falseLabel = element->Attribute("false");
        Label* f = falseLabel ? parseLabel(falseLabel) : nullptr;
        
        return new QuadCJump(nullptr, relop, left, right, t, f, def, use);
    } 
    else if (strcmp(kind, "PHI") == 0) {
        const char* dstName = element->Attribute("dst");
        const char* dstType = element->Attribute("temp_type");
        TempExp* temp = dstName ? parseTempExp(dstName, dstType) : nullptr;
        
        vector<pair<Temp*, Label*>>* args = new vector<pair<Temp*, Label*>>();
        XMLElement* argsElem = element->FirstChildElement("args");
        if (argsElem) {
            for (XMLElement* argElem = argsElem->FirstChildElement("arg"); 
                 argElem; 
                 argElem = argElem->NextSiblingElement("arg")) {
                const char* tempName = argElem->Attribute("temp");
                const char* labelName = argElem->Attribute("label");
                
                if (tempName && labelName) {
                    Temp* temp = nullptr;
                    if (tempMap.find(tempName) != tempMap.end()) {
                        temp = tempMap[tempName];
                    } else {
                        // Extract number from temp name (format: t123)
                        int tempNum = 0;
                        if (tempName[0] == 't') {
                            tempNum = stoi(tempName + 1);
                        } else {
                            tempNum = stoi(tempName);
                        }
                        
                        temp = new Temp(tempNum);
                        tempMap[tempName] = temp;
                    }
                    
                    Label* label = parseLabel(labelName);
                    if (temp && label) {
                        args->push_back(make_pair(temp, label));
                    }
                }
            }
        }
        
        return new QuadPhi(nullptr, temp, args, def, use);
    } 
    else if (strcmp(kind, "RETURN") == 0) {
        XMLElement* valueElem = element->FirstChildElement("value");
        QuadTerm* value = valueElem ? parseQuadTerm(valueElem) : nullptr;
        
        return new QuadReturn(nullptr, value, def, use);
    }
    
    cerr << "Unknown statement kind: " << kind << endl;
    return nullptr;
}

// Parse a quad term
static QuadTerm* parseQuadTerm(XMLElement* element) {
    const char* kind = element->Attribute("kind");
    if (!kind) {
        cerr << "QuadTerm missing kind attribute" << endl;
        return nullptr;
    }
    
    if (strcmp(kind, "TEMP") == 0) {
        const char* id = element->Attribute("id");
        const char* typeStr = element->Attribute("type");
        if (!id) {
            cerr << "TEMP term missing id attribute" << endl;
            return nullptr;
        }
        
        TempExp* temp = parseTempExp(id, typeStr);
        return new QuadTerm(temp);
    } 
    else if (strcmp(kind, "CONST") == 0) {
        int value = 0;
        element->QueryIntAttribute("value", &value);
        return new QuadTerm(value);
    } 
    else if (strcmp(kind, "NAME") == 0) {
        const char* name = element->Attribute("name");
        if (!name) {
            cerr << "NAME term missing name attribute" << endl;
            return nullptr;
        }
        
        return new QuadTerm(string(name));
    }
    
    cerr << "Unknown QuadTerm kind: " << kind << endl;
    return nullptr;
}

// Parse a TempExp from a temp name
static TempExp* parseTempExp(const char* tempName, const char* typeStr) {
    if (!tempName) return nullptr;
    
    // Extract number from temp name (format: t123)
    int tempNum = 0;
    if (tempName[0] == 't') {
        tempNum = stoi(tempName + 1);
    } else {
        tempNum = stoi(tempName);
    }
    
    // Reuse existing Temp or create a new one
    Temp* temp = nullptr;
    if (tempMap.find(tempName) != tempMap.end()) {
        temp = tempMap[tempName];
    } else {
        temp = new Temp(tempNum);
        tempMap[tempName] = temp;
    }
    
    // Convert string to type enum
    tree::Type type = stringToType(typeStr);
    
    return new TempExp(type, temp);
}

// Parse def or use set from XML
static set<Temp*>* parseDefUse(XMLElement* element, const char* defOrUse) {
    set<Temp*>* result = new set<Temp*>();
    
    XMLElement* setElem = element->FirstChildElement(defOrUse);
    if (!setElem) {
        return result; // Empty set
    }
    
    for (XMLElement* tempElem = setElem->FirstChildElement("temp"); 
         tempElem; 
         tempElem = tempElem->NextSiblingElement("temp")) {
        const char* id = tempElem->Attribute("id");
        if (id) {
            Temp* temp = nullptr;
            if (tempMap.find(id) != tempMap.end()) {
                temp = tempMap[id];
            } else {
                // Extract number from temp name (format: t123)
                int tempNum = 0;
                if (id[0] == 't') {
                    tempNum = stoi(id + 1);
                } else {
                    tempNum = stoi(id);
                }
                
                temp = new Temp(tempNum);
                tempMap[id] = temp;
            }
            
            if (temp) {
                result->insert(temp);
            }
        }
    }
    
    return result;
}

// Parse a label from a string
static Label* parseLabel(const char* labelStr) {
    if (!labelStr) return nullptr;
    
    // Check if we already have this label
    if (labelMap.find(labelStr) != labelMap.end()) {
        return labelMap[labelStr];
    }
    
    // Create a new label
    int labelNum = stoi(labelStr);
    Label* label = new Label(labelNum);
    labelMap[labelStr] = label;
    
    return label;
}