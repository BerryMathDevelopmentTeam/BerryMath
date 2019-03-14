#include <iostream>
#include <regex>
#include "memory.h"
#include "AST.h"
#include "script.h"

void BerryMath::script::run() {
    int i = 0;
    auto scope = new block();
    string c("");
    std::regex replacer("\n");
    code = std::regex_replace(code, replacer, "//\n");
    int bigBracketsCount = 0;
    bool haveEndToken = false;
//    std::cout << code << std::endl;
    while (true) {
        if (i >= code.length()) break;
        c += code[i];
//        std::cout << code[i];
        if (code[i] == '{') bigBracketsCount++;
        if (code[i] == '}') bigBracketsCount--;
//        std::cout << bigBracketsCount << std::endl;
        if (code[i] == ';' && bigBracketsCount == 0) {
            auto ast = new BerryMath::AST(c);
            ast->parse();
            auto root = ast->value();
//            std::cout << root->value() << std::endl;
//            if (root->at(0)->value() == "set") {
//                auto name = root->at(0)->at(0)->value();
//                if (scope->of(name) != nullptr) {
//                    delete scope->variables[name];
////                    scope->variables[name] = new variable(name, eval(root->at(0)->at(1), *scope));
//                } else {
////                    scope->insert(new variable(name, eval(root->at(0)->at(1), *scope)));
//                }
//            }
            delete ast;
            c = "";
        }
        i++;
    }
//    std::cout << "[Program finish]" << std::endl;
//    std::cout << "=========Hash Table=========" << std::endl;
//    scope->each([](variable* var) -> void {
//        std::cout << var->nameOf() << ": " << var->valueOf().valueOf() << std::endl;
//    });
//    std::cout << "=======Hash Table End=======" << std::endl;
}