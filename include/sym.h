#ifndef BERRYMATH_SYM_H
#define BERRYMATH_SYM_H

#include <string>
#include <map>
#include "types.h"
using std::string;

namespace BM {
    typedef std::map<string, long long> Symtb;
    class Sym {
    public:
        Sym(Sym* = nullptr);
        void push(const string&);
        Symtb::iterator get(const string&);
        Symtb::iterator end() { return symtb.end(); }
        inline void clear() { symtb.clear();top = 0; }
        void reset(Sym* p) { clear(); parent = p; }
    private:
        ULL top = 0;
        Sym* parent;
        Symtb symtb;
    };
}


#endif //BERRYMATH_SYM_H
