#ifndef __QUAD_SSA_HH
#define __QUAD_SSA_HH

#include <map>
#include <set>
#include <string>
#include <vector>
#include "temp.hh"
#include "treep.hh"
#include "quad.hh"

using namespace std;
using namespace quad;

class VersionedTemp {
public:

//Here we use a rather simper method to generate temps with versions
//with the assumption that the number of versions is small (<=100)

static int versionedTempNum(int old_num, int version) {
    return old_num*100+version; //unique temp for each version
}

static int origTempNum(int versionedTempNum) {
    return versionedTempNum/100; //original temp number
}

//more sophisticated implementation can be done by remembering
//the num of each new versions, and using a map to get the 
//original temp num.
};

QuadProgram* quad2ssa(QuadProgram* program);

#endif // __QUAD_SSA_HH
