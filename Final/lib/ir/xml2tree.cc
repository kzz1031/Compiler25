#define DEBUG
#undef DEBUG

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "temp.hh"
#include "treep.hh"
#include "xml2tree.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace tree;

static Temp_map *temp_map;

//forward declarations
static tree::Program* parseProgram(XMLElement* element);
static tree::FuncDecl* parseFuncDecl(XMLElement* element);
static tree::Block* parseBlock(XMLElement* element);
static tree::Jump* parseJump(XMLElement* element);
static tree::Cjump* parseCjump(XMLElement* element);
static tree::Move* parseMove(XMLElement* element);
static tree::Seq* parseSeq(XMLElement* element);
static tree::LabelStm* parseLabelStm(XMLElement* element);
static tree::Return* parseReturn(XMLElement* element);
static tree::Phi* parsePhi(XMLElement* element);
static tree::ExpStm* parseExpStm(XMLElement* element);
static tree::Const* parseConst(XMLElement* element);
static tree::Name* parseName(XMLElement* element);
static tree::TempExp* parseTempExp(XMLElement* element);
static tree::Eseq* parseEseq(XMLElement* element);
static tree::Mem* parseMem(XMLElement* element);
static tree::Binop* parseBinop(XMLElement* element);
static tree::Call* parseCall(XMLElement* element);
static tree::ExtCall* parseExtCall(XMLElement* element);

tree::Program* xml2tree(XMLDocument* doc) {
    XMLElement* root = doc->RootElement();
    temp_map = new Temp_map();
    if (!root || string(root->Name()) != "Program") {
        throw runtime_error("Invalid XML format: Root element is not 'Program'");
    }
    return parseProgram(root);
}

tree::Program* xml2tree(string xmlfilename) {
    XMLDocument doc;
    if (doc.LoadFile(xmlfilename.c_str()) != XML_SUCCESS) {
        cerr << "Error: " << xmlfilename << " not found" << endl;
        return nullptr;
    }
    if (doc.ErrorID() != XML_SUCCESS) {
        cerr << "Error: " << xmlfilename << " is not a valid XML file" << endl;
        return nullptr;
    }
    return xml2tree(&doc);
}

static tree::Type stringToType(const string& typeStr) {
#ifdef DEBUG
    cout << "Converting string to type: " << typeStr << endl;
#endif
    if (string(typeStr) == "INT") return tree::Type::INT;
    if (string(typeStr) == "PTR") return tree::Type::PTR;
    throw runtime_error("Unknown type: " + typeStr);
}

static Temp * stringToTemp(const string& tempStr) {
#ifdef DEBUG
    cout << "Converting string to temp: " << tempStr << endl;
#endif
    int tempId = stoi(tempStr);
    return temp_map->named_temp(tempId);
}

static Label * stringToLabel(const string& labelStr) {
#ifdef DEBUG
    cout << "Converting string to label: " << labelStr << endl;
#endif
    int labelId = stoi(labelStr);
    if (labelId == -1) {
        return temp_map->named_label(labelId);
    }
    return nullptr;
}

static String_Label * stringToStringLabel(const string& labelStr) {
#ifdef DEBUG
    cout << "Converting string to string label: " << labelStr << endl;
#endif
    return temp_map->newstringlabel(labelStr);
}

static tree::Stm* parseStm(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing statement: " << element->Name() << endl;
#endif
    string name = element->Name();
    if (name == "Jump") {
        return parseJump(element);
    } else if (name == "CJump") {
        return parseCjump(element);
    } else if (name == "Move") {
        return parseMove(element);
    } else if (name == "Sequence") {
        return parseSeq(element);
    } else if (name == "Label") {
        return parseLabelStm(element);
    } else if (name == "Return") {
        return parseReturn(element);
    } else if (name == "Phi") {
        return parsePhi(element);
    } else if (name == "ExpressionStatement") {
        return parseExpStm(element);
    }
    throw runtime_error("Unknown statement type: " + name);
}

static tree::Exp* parseExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing expression: " << element->Name() << endl;
#endif
    string name = element->Name();
    if (name == "Const") {
        return parseConst(element);
    } else if (name == "Name") {
        return parseName(element);
    } else if (name == "Temp") {
        return parseTempExp(element);
    } else if (name == "ESeq") {
        return parseEseq(element);
    } else if (name == "Memory") {
        return parseMem(element);
    } else if (name == "BinOp") {
        return parseBinop(element);
    } else if (name == "Call") {
        return parseCall(element);
    } else if (name == "ExtCall") {
        return parseExtCall(element);
    }
    throw runtime_error("Unknown expression type: " + name);
}

#define FuncDeclList vector<FuncDecl*>

static tree::Program* parseProgram(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Program" << endl;
#endif
    auto funcdecllist = new FuncDeclList();
    for (XMLElement* child = element->FirstChildElement("FunctionDeclaration"); child; 
                                    child = child->NextSiblingElement("FunctionDeclaration")) {
#ifdef DEBUG
        cout << "--- Func name=" << child->Attribute("name") << endl;
#endif
        funcdecllist->push_back(parseFuncDecl(child));
#ifdef DEBUG
        cout << "--- Done Func name=" << child->Attribute("name") << endl;
#endif
    }
#ifdef DEBUG
    cout << "Parsed " << funcdecllist->size() << " function declarations" << endl;
#endif
    return new Program(funcdecllist);
}

static tree::FuncDecl* parseFuncDecl(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Function Declaration" << endl;
#endif
    string name = element->Attribute("name");
    tree::Type return_type = stringToType(element->Attribute("return_type"));
    int last_temp = element->IntAttribute("last_temp");
    int last_label = element->IntAttribute("last_label");

    // Parse arguments
    auto args = new vector<tree::Temp*>();
    for (int i = 0; ; i++) {
        const char* arg_temp = element->Attribute(("arg_temp_" + to_string(i)).c_str());
        if (!arg_temp) break;
        args->push_back(stringToTemp(arg_temp));
    }

    // Parse blocks
    auto blocks = new vector<tree::Block*>();
    XMLElement* blocksElement = element->FirstChildElement("Blocks");
    if (blocksElement) {
        for (XMLElement* blockElement = blocksElement->FirstChildElement("Block"); 
                blockElement; blockElement = blockElement->NextSiblingElement("Block")) {
            blocks->push_back(parseBlock(blockElement));
        }
    } else {
        throw runtime_error("No blocks found in function declaration");
    }

    auto func = new tree::FuncDecl(name, args, blocks, return_type, last_temp, last_label);
    return func;
}

static tree::Block* parseBlock(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Block" << endl;
#endif
    string entry_label = element->Attribute("entry_label");

    // Parse exit labels
    auto exit_labels = new vector<tree::Label*>();
    for (int i=0; ; i++) {
        const char* exit_label = element->Attribute(("exit_label_" + to_string(i)).c_str());
        if (!exit_label) break;
        exit_labels->push_back(stringToLabel(exit_label));
    }

    // Parse statements
    XMLElement* stmsElement = element->FirstChildElement("Sequence");
    tree::Seq* seq = parseSeq(stmsElement);

    return new tree::Block(temp_map->named_label(stoi(entry_label)), exit_labels, seq->sl);
}

tree::Jump* parseJump(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Jump" << endl;
#endif
    string label = element->Attribute("label");
    return new tree::Jump(temp_map->named_label(stoi(label)));
}

tree::Cjump* parseCjump(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Cjump" << endl;
#endif
    string relop = element->Attribute("relop");
    string trueLabel = element->Attribute("true");
    string falseLabel = element->Attribute("false");

    auto e = element->FirstChildElement();
    auto left = parseExp(e);
    e = e->NextSiblingElement();
    auto right = parseExp(e);

    return new tree::Cjump(relop, left, right, 
           temp_map->named_label(stoi(trueLabel)), temp_map->named_label(stoi(falseLabel)));
}

tree::Move* parseMove(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Move" << endl;
#endif
    auto e2 = element->FirstChildElement();
    auto dst = parseExp(e2);
    e2 = e2->NextSiblingElement();
    auto src = parseExp(e2);
    return new tree::Move(dst, src);
}

tree::Seq* parseSeq(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Seq" << endl;
#endif
    auto statements = new vector<tree::Stm*>();
    for (XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
#ifdef DEBUG
        cout << "---child name=" << child->Name() << endl;
#endif
        statements->push_back(parseStm(child));
    }
    return new tree::Seq(statements);
}

tree::LabelStm* parseLabelStm(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing LabelStm" << endl;
#endif
    const char* label = element->Attribute("label");
    return new tree::LabelStm(temp_map->named_label(stoi(label)));
}

tree::Return* parseReturn(XMLElement* element) {
    XMLElement* expElement = element->FirstChildElement();
    auto exp = parseExp(expElement);
    return new tree::Return(exp);
}

tree::Phi* parsePhi(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Phi" << endl;
#endif
    auto temp = temp_map->named_temp(stoi(element->Attribute("define")));
    auto args = new vector<pair<tree::Temp*, tree::Label*>>();
    for (int i=0; true; i++) {
        const char* arg = element->Attribute(("temp_"+to_string(i)).c_str());
        const char* from = element->Attribute(("from_"+to_string(i)).c_str());
        if (!arg || !from) break;
        args->emplace_back(temp_map->named_temp(stoi(arg)), temp_map->named_label(stoi(from)));
    }
    return new tree::Phi(temp, args);
}

tree::ExpStm* parseExpStm(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing ExpStm" << endl;
#endif
    XMLElement* expElement = element->FirstChildElement();
    if (!expElement) {
        throw runtime_error("No expression found in ExpStm");
    }
    return new tree::ExpStm(parseExp(expElement));
}

tree::Const* parseConst(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Const" << endl;
#endif
    const char* valueStr = element->Attribute("value");
    if (!valueStr) {
        throw runtime_error("No value found in Const element");
    }
    int value = element->IntAttribute("value");
    return new tree::Const(value);
}

tree::Name* parseName(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Name" << endl;
#endif
    const char* name = element->Attribute("name");
    string s = string(name);
    if (!name) {
        throw runtime_error("No name found in Name element");
    }
    // Check if it's a string label
    bool is_num = !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    if (is_num) {
        return new tree::Name(stringToLabel(name));
    } else {
        return new tree::Name(stringToStringLabel(name));
    }
}

tree::TempExp* parseTempExp(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing TempExp" << endl;
#endif
    const char *temp = element->Attribute("temp");
    const char *type = element->Attribute("type");
    if (!temp || !type) {
        throw runtime_error("No temp or type found in TempExp element");
    }
    return new tree::TempExp( (string(type) == "INT" ? tree::Type::INT : tree::Type::PTR),
                temp_map->named_temp(stoi(temp)));
}

tree::Eseq* parseEseq(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Eseq" << endl;
#endif
    auto e = element->FirstChildElement();
    auto stm = parseStm(e);
    if (!stm) {
        throw runtime_error("No statement found in Eseq element");
    }
    auto exp = parseExp(e->NextSiblingElement());
    if (!exp) {
        throw runtime_error("No expression found in Eseq element");
    }
    return new tree::Eseq(exp->type, stm, exp);
}

tree::Mem* parseMem(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Mem" << endl;
#endif
    auto t_type = stringToType(element->Attribute("type"));
    if (t_type != tree::Type::INT && t_type != tree::Type::PTR) {
        throw runtime_error("Invalid type for Mem element");
    }
    auto addr = parseExp(element->FirstChildElement());
    if (!addr) {
        throw runtime_error("No address found in Mem element");
    }
#ifdef DEBUG
    cout << "Mem address type: " << (addr->type == tree::Type::INT ? "INT" : "PTR") << endl;
#endif
    return new tree::Mem(t_type, addr);
}

tree::Binop* parseBinop(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Binop" << endl;
#endif
    string op = element->Attribute("op");
    auto type = stringToType(element->Attribute("type"));
    auto e = element->FirstChildElement();
    auto left = parseExp(e);
    e = e->NextSiblingElement();
    auto right = parseExp(e);
    return new tree::Binop(type, op, left, right);
}

tree::Call* parseCall(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing Call" << endl;
#endif
    string id = element->Attribute("id");
    Type type = stringToType(element->Attribute("type"));
    auto obj = parseExp(element->FirstChildElement());
    auto args = new vector<tree::Exp*>();
    XMLElement* argsElement = element->FirstChildElement("Arguments");
    for (XMLElement* argElement = argsElement->FirstChildElement(); argElement; argElement = argElement->NextSiblingElement()) {
        args->push_back(parseExp(argElement));
    }
    return new tree::Call(type, id, obj, args);
}

tree::ExtCall* parseExtCall(XMLElement* element) {
#ifdef DEBUG
    cout << "Parsing ExtCall" << endl;
#endif
    string extfun = element->Attribute("extfun");
    Type type = stringToType(element->Attribute("type"));
    auto args = new vector<tree::Exp*>();
    XMLElement* argsElement = element->FirstChildElement("Arguments");
    for (XMLElement* argElement = argsElement->FirstChildElement(); argElement; argElement = argElement->NextSiblingElement()) {
        args->push_back(parseExp(argElement));
    }

    return new tree::ExtCall(type, extfun, args);
}