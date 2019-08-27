#include "sym.h"

BM::Sym::Sym(Sym* p) : parent(p) {
    if (parent) {
        auto sb = -parent->top;
        for (auto i = parent->symtb.begin(); i != parent->symtb.end(); i++) {
            symtb.insert(std::pair<string, long long>(i->first, sb + i->second));
        }
    }
}
void BM::Sym::push(const string& s) {
    symtb.insert(std::pair<string, long long>(s, top++));
}
BM::Symtb::iterator BM::Sym::get(const string& s) {
    return symtb.find(s);;
}