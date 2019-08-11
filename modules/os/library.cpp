#include <string>
#include <cstdlib>
#include "library.h"

Object* system(BM::Scope* scope, vector<Object*> unknowns) {
    auto cmdBMSV = scope->get("cmd");
    if (!cmdBMSV) return new Number(-1);
    string cmd(((String*)cmdBMSV->value())->value());
    return new Number(system(cmd.c_str()));
}

Object* initModule() {
    auto exports = new Object;

    auto systemP = new NativeFunction("system", system);
    systemP->addDesc("cmd");
    exports->set("system", systemP);

    return exports;
}