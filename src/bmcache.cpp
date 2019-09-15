#include <iostream>
#include "value.h"
#include "ast.h"
#include "bmcache.h"

void* operator new(size_t sz) {
    void* o = malloc(sz);
    switch (sz) {
        case sizeof(BM::Object):
        case sizeof(BM::Number):
        case sizeof(BM::String):
        case sizeof(BM::AST):
            auto iter = BM::deleted.find(o);
            if (iter != BM::deleted.end()) BM::deleted.erase(iter);
            break;
    }
    return o;
}
void operator delete(void* o) {
    auto iter = BM::deleted.find(o);
    if (iter == BM::deleted.end()) {
        BM::deleted[o] = true;
        free(o);
    }
}