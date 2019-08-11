#include <iostream>
#include <string>
#include <cstdlib>
#include "library.h"

Object* FileCtor(BM::Scope* scope, vector<Object*> unknowns) {
    auto self = new Object;
    self->set("path", (String*)scope->get("path")->value());
    self->set("type", (String*)scope->get("type")->value());
    return self;
}

Object* initModule() {
    auto exports = new Object;

    auto FileClass = new Object;
    auto prototype = new Object;

    FileClass->set("prototype", prototype);

    auto FileCtorP = new NativeFunction("ctor", FileCtor);
    FileCtorP->addDesc("path");
    FileCtorP->addDesc("type");
    FileCtorP->defaultValue("type", new String("file"));
    prototype->set("ctor", FileCtorP);

    exports->set("File", FileClass);
    return exports;
}