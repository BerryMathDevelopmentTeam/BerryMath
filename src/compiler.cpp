#include <iostream>
#include "compiler.h"

string BM::Compiler::compile() {
    if (!ast) ast = new AST;
    ast->open(script);
    string bytecode;
    while (true) {
        ast->parse();
        if (ast->value() == "PROGRAM-END") break;
        if (ast->value() == "bad-tree") {
            std::cerr << ast->rValue()->get(0)->value() << "\n\tat <" << filename << ">:" << ast->line() << std::endl;
            if (ast) { delete ast;ast = nullptr; }
            return "";
        }
        if (ast->value() == "let") {
        }
    }
    return bytecode;
}