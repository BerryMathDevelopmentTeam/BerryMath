#include "library.h"

#include <iostream>

Object* creatArray() {
    auto Array = new BM::NativeClass("Array");
    Array->addProto("ctor", new BM::NativeFunction("ctor", [](Scope* scope, vector<Object*> argv) -> Object* {
        auto selfV = scope->get("this");
        if (!selfV) return new BM::Undefined;
        auto self = selfV->value();
        if (!self) return new BM::Undefined;
        // std::cout << self->toString() << std::endl;
        self->set("__len", new BM::Number(0));
        self->set("__SYSTEM_TYPE__", new BM::String("Array"));
        return self;
    }));
    Array->addProto("push", new BM::NativeFunction("push", [](Scope* scope, vector<Object*> argv) -> Object* {
        auto selfV = scope->get("this");
        if (!selfV) return new BM::Undefined;
        auto self = selfV->value();
        if (!self) return new BM::Undefined;
        double& len = ((BM::Number*)self->get("__len"))->value();
        for (unsigned long i = 0; i < argv.size(); i++) {
            self->set(std::to_string(i + len), argv[i]->type() == BM::OBJECT ? argv[i] : argv[i]->copy());
        }
        len += argv.size();
        return self;
    }));
    return Array;
}

Object* initModule() {
    auto module = new Object;
    module->set("Array", creatArray());
    return module;
}