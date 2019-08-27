#include <iostream>
#include "compiler.h"

string BM::Compiler::compile() {
    if (!child) {
        if (!ast) ast = new AST;
        ast->open(script);
    }
    string bytecode;
    while (true) {
        if (!child) {
            ast->parse();
        }
        if (ast->value() == "PROGRAM-END") break;
        if (ast->value() == "bad-tree") {
            std::cerr << ast->rValue()->get(0)->value() << "\n\tat <" << filename << ">:" << ast->line() << std::endl;
            if (ast) { delete ast;ast = nullptr; }
            return "";
        }
        if (ast->value() == "let") {
            string name(ast->rValue()->get(0)->value());
            Compiler compiler("", filename, this);
            compiler.ast = new AST();
            compiler.ast->root = ast->rValue()->get(1);
        } else {
            if (ast->value() == "call") {
            } else if (ast->value() == "get") {
            } else if (ast->value() == ".") {
            } else if (ast->value() == "o-value") {
            } else if (ast->value() == "a-value") {
            } else {
                auto len = ast->rValue()->length();
                switch (len) {
                    case 0:
                    {
                        if (isNumber(ast->value())) {
                            bytecode += (char)PUSH;
                            bytecode += (char)NUM;
                            bytecode += transI64S_bin(transDI64_bin(transSD(ast->value())));
                        } else if (isString(ast->value())) {
                            auto s = ast->value();
                            s.erase(0, 1);
                            s.erase(s.length() - 1, 1);
                            bytecode += (char)PUSH;
                            bytecode += (char)STR;
                            bytecode += s;
                            bytecode += (char)0;
                        } else if (ast->value() == "null") {
                            bytecode += (char)PUSH;
                            bytecode += (char)NUL;
                            bytecode += (char)0;
                        } else if (ast->value() == "undefined") {
                            bytecode += (char)PUSH;
                            bytecode += (char)UND;
                            bytecode += (char)0;
                        } else {
                            auto i = sym.get(ast->value());
                            if (i == sym.end()) {
                                std::cerr << "NameError: use of undeclared identifier '" << ast->value() << "'\n\tat <" << filename << ">:" << ast->line() << std::endl;
                                if (ast) { delete ast;ast = nullptr; }
                                return "";
                            } else {
                            }
                        }
                        break;
                    }
                    default:
                    {
                        std::cerr << "ASTError: Unexpected AST.\n\tat <" << filename << ">:" << ast->line() << std::endl;
                        if (ast) { delete ast;ast = nullptr; }
                        return "";
                    }
                }
            }
        }
        if (child) break;
    }
    return bytecode;
}