#ifndef _AST_LOCATION
#define _AST_LOCATION

#include <cstddef>
#include <ostream>
#include <utility>

namespace fdmj 
{
    class ast_location
    {
        public:
        std::size_t sline, scolumn, eline, ecolumn; //s-start, e-end: start/end line, and columns
        ast_location(std::size_t sline, std::size_t scolumn, std::size_t eline, std::size_t ecolumn) : 
            sline(sline), scolumn(scolumn), eline(eline), ecolumn(ecolumn) {}
        ast_location() : ast_location(0, 0, 0, 0) {}
    };

    using position_t = std::size_t;;
    using location_t = fdmj::ast_location;
};

inline std::ostream& operator<<(std::ostream& os, const fdmj::ast_location& loc)
{
    return os << "[" << loc.sline<< ":" << loc.scolumn<< "-" << loc.eline<<":" <<loc.ecolumn<< "]";
}

#endif
