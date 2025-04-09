#include <string>
#include "treep.hh"

namespace tree { 
std::string kindToString(Kind kind) {
    switch (kind) {
        case Kind::PROGRAM:
            return "PROGRAM";
        case Kind::FUNCDECL:
            return "FUNCDECL";
        case Kind::BLOCK:
            return "BLOCK";
        case Kind::JUMP:
            return "JUMP";
        case Kind::CJUMP:
            return "CJUMP";
        case Kind::MOVE:
            return "MOVE";
        case Kind::SEQ:
            return "SEQ";
        case Kind::LABELSTM:
            return "LABEL";
        case Kind::RETURN:
            return "RETURN";
        case Kind::PHI:
            return "PHI";
        case Kind::EXPSTM:
            return "EXPSTM";
        case Kind::BINOP:
            return "BINOP";
        case Kind::MEM:
            return "MEM";
        case Kind::TEMPEXP:
            return "TEMP";
        case Kind::ESEQ:
            return "ESEQ";
        case Kind::NAME:
            return "NAME";
        case Kind::CONST:
            return "CONST";
        case Kind::CALL:
            return "CALL";
        case Kind::EXTCALL:
            return "EXTCALL";
        default:
            return "UNKNOWN_KIND";
    }
}
std::string typeToString(tree::Type type) {
    switch(type) {
      case Type::INT: return "INT";
      case Type::PTR: return "PTR";
    }
    return "UNKNOWN";
}

} //namespace tree