#include <iostream>
#include "compiler.h"

bool BM::Compiler::compile(string& bytecode) {
    if (!child) {
        if (!ast) ast = new AST;
        ast->open(script);
    }
    bytecode = "";
    while (true) {
        if (!child) ast->parse();
        if (ast->value() == "bad-tree") {
            std::clog << ast->rValue()->get(0)->value() << "\n\tat <" << filename << ">:" << ast->line() << std::endl;
            FREEAST
            return false;
        }
        if (ast->value() == "let") {
            string name(ast->rValue()->get(0)->value());
            Compiler compiler("", filename, this);
            compiler.ast = new AST();
            compiler.ast->root = ast->rValue()->get(1);
            compiler.child = true;
            string t;
            COMPILE(compiler, t)
            sym.push(name);
        }
        if (child) break;
    }
    return true;
}