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
    UL i = 0;
    for (; i < unknowns.size() - 1; i++) {
        std::cout << unknowns[i]->toString(true, false) << sp;
    }
    std::cout << unknowns[i]->toString(true, false) << scope->get("end")->value()->toString(true, false);
    return new BM::Undefined;
}
Object* input(BM::Scope* scope, vector<Object*> unknowns) {
    auto sp = scope->get("sp")->value()->toString(true, false);
    UL i = 0;
    for (; i < unknowns.size() - 1; i++) {
        std::cout << unknowns[i]->toString(true, false) << sp;
    }
    std::cout << unknowns[i]->toString(true, false);
    string t;
    std::cin >> t;
    return new BM::String(t);
}

BM::Object* initModule() {
    auto exports = new BM::Object;
    auto printP = new BM::NativeFunction(print);
    auto inputP = new BM::NativeFunction(input);
    printP->defaultValue("end", new BM::String("\n"));
    printP->defaultValue("sp", new BM::String(" "));
    inputP->defaultValue("sp", new BM::String(" "));
    exports->set("io", new BM::Object);
    exports->get("io")->set("print", printP);
    exports->get("io")->set("input", inputP);
    return exports;
}