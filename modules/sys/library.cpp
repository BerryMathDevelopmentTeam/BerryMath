#include <iostream>
#include <BerryMath.h>
#include <vector>
#include <map>
#include "library.h"
using std::vector;
using std::map;
using BM::Object;
using BM::Scope;

Object* print(BM::Scope* scope, vector<Object*> unknowns) {
    auto sp = scope->get("sp")->value()->toString(true, false);
    auto end = scope->get("end")->value()->toString(true, false);
    for (UL i = 0; i < unknowns.size(); i++) {
        std::cout << unknowns[i]->toString(true, false) << sp;
    }
    std::cout << end;
    return new BM::Undefined;
}

BM::Object* initModule() {
    auto exports = new BM::Object;
    exports->set("PI", new BM::Number(3.1415));
    auto printP = new BM::NativeFunction(print);
    printP->defaultValue("end", new BM::String("\n"));
    printP->defaultValue("sp", new BM::String(" "));
    exports->set("print", printP);
    return exports;
}