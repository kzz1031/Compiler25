#ifndef __QUAD2XML_HH
#define __QUAD2XML_HH

#include <iostream>
#include <string>
#include <vector>
#include "quad.hh"

using namespace std;
using namespace quad;
using namespace tinyxml2;

// Forward declaration of Quad2XML visitor class
class Quad2XML : public QuadVisitor {
private:
    XMLDocument* doc;
    XMLElement* current;

    // Helper methods
    XMLElement* createElementWithKind(const char* elemName, QuadKind kind);
    void addTempAttribute(XMLElement* elem, const char* attrName, TempExp* temp);
    void addLabelAttribute(XMLElement* elem, const char* attrName, Label* label);
    void addQuadTermElement(XMLElement* parent, const char* elemName, QuadTerm* term);
    void processDefUse(XMLElement* elem, set<Temp*>* def, set<Temp*>* use);

public:
    explicit Quad2XML(XMLDocument* doc);
    ~Quad2XML() = default;

    // Visitor methods for each Quad type
    void visit(QuadProgram* program) override;
    void visit(QuadFuncDecl* funcdecl) override;
    void visit(QuadBlock* block) override;
    void visit(QuadMove* move) override;
    void visit(QuadLoad* load) override;
    void visit(QuadStore* store) override;
    void visit(QuadMoveBinop* movebinop) override;
    void visit(QuadCall* call) override;
    void visit(QuadMoveCall* movecall) override;
    void visit(QuadExtCall* extcall) override;
    void visit(QuadMoveExtCall* moveextcall) override;
    void visit(QuadLabel* label) override;
    void visit(QuadJump* jump) override;
    void visit(QuadCJump* cjump) override;
    void visit(QuadPhi* phi) override;
    void visit(QuadReturn* returnQuad) override;
};

// Function to convert a Quad program to an XML Document
bool quad2xml(QuadProgram* program, const char* filename);

#endif // __QUAD2XML_HH