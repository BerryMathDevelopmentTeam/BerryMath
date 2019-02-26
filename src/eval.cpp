#include <regex>
#include "eval.h"

BerryMath::value* BerryMath::eval(AST::ASTNode* ast, block& variables) {
    if (ast->type() == AST::OPERATOR) {
//        std::cout << ast->value() << std::endl;
        if (ast->value() == "expression") {
            return eval(ast->at(0), variables);
        } else {
//            std::cout << ast->value() << std::endl;
            if (ast->value() == "add") {
                auto a = eval(ast->at(0), variables);
                auto b = eval(ast->at(1), variables);
//                std::cout << a->typeOf() << ", " << a->valueOf() << std::endl;
//                std::cout << b->typeOf() << ", " << b->valueOf() << std::endl;
                if (a->typeOf() == STRING) {
                    string v_a = a->valueOf();
                    string v_b;
                    v_a.erase(0);
                    v_a.erase(v_a.length() - 1);
                    if (b->typeOf() == NUMBER) {
                        v_b = b->valueOf();
                    } else if (b->typeOf() != STRING) {
                        return new value(UNDEFINED, "undefined");
                    } else {
                        v_b.erase(0);
                        v_b.erase(v_b.length() - 1);
                    }
                    string v_c("\"" + v_a + v_b + "\"");
                    delete a;
                    delete b;
                    return new value(STRING, v_c);
                }
                if (b->typeOf() == STRING) {
                    string v_a;
                    string v_b = b->valueOf();
                    v_b.erase(0);
                    v_b.erase(v_b.length() - 1);
                    if (a->typeOf() == NUMBER) {
                        v_a = a->valueOf();
                    } else if (a->typeOf() != STRING) {
                        return new value(UNDEFINED, "undefined");
                    } else {
                        v_a.erase(0);
                        v_a.erase(v_b.length() - 1);
                    }
                    string v_c("\"" + v_a + v_b + "\"");
                    delete a;
                    delete b;
                    return new value(STRING, v_c);
                }
                if (a->typeOf() != NUMBER || b->typeOf() != NUMBER) {
                    return new value(UNDEFINED, "undefined");
                }
                double a_res = atof(a->valueOf().c_str());
                double b_res = atof(b->valueOf().c_str());
                double c_res = a_res + b_res;
                return new value(NUMBER, std::to_string(c_res));
            }
            if (ast->value() == "mul") {
                auto a = eval(ast->at(0), variables);
                auto b = eval(ast->at(1), variables);
//                std::cout << a->typeOf() << ", " << a->valueOf() << std::endl;
//                std::cout << b->typeOf() << ", " << b->valueOf() << std::endl;
                if (a->typeOf() != NUMBER || b->typeOf() != NUMBER) {
                    return new value(UNDEFINED, "undefined");
                }
                double a_res = atof(a->valueOf().c_str());
                double b_res = atof(b->valueOf().c_str());
                double c_res = a_res * b_res;
                return new value(NUMBER, std::to_string(c_res));
            }
            if (ast->value() == "sub") {
                auto a = eval(ast->at(0), variables);
                auto b = eval(ast->at(1), variables);
//                std::cout << a->typeOf() << ", " << a->valueOf() << std::endl;
//                std::cout << b->typeOf() << ", " << b->valueOf() << std::endl;
                if (a->typeOf() != NUMBER || b->typeOf() != NUMBER) {
                    return new value(UNDEFINED, "undefined");
                }
                double a_res = atof(a->valueOf().c_str());
                double b_res = atof(b->valueOf().c_str());
                double c_res = a_res - b_res;
                return new value(NUMBER, std::to_string(c_res));
            }
            if (ast->value() == "div") {
                auto a = eval(ast->at(0), variables);
                auto b = eval(ast->at(1), variables);
//                std::cout << a->typeOf() << ", " << a->valueOf() << std::endl;
//                std::cout << b->typeOf() << ", " << b->valueOf() << std::endl;
                if (a->typeOf() != NUMBER || b->typeOf() != NUMBER) {
                    return new value(UNDEFINED, "undefined");
                }
                double a_res = atof(a->valueOf().c_str());
                double b_res = atof(b->valueOf().c_str());
                double c_res = a_res / b_res;
                return new value(NUMBER, std::to_string(c_res));
            }
            if (ast->value() == "mod") {
                auto a = eval(ast->at(0), variables);
                auto b = eval(ast->at(1), variables);
//                std::cout << a->typeOf() << ", " << a->valueOf() << std::endl;
//                std::cout << b->typeOf() << ", " << b->valueOf() << std::endl;
                if (a->typeOf() != NUMBER || b->typeOf() != NUMBER) {
                    return new value(UNDEFINED, "undefined");
                }
                int a_res = atof(a->valueOf().c_str());
                int b_res = atof(b->valueOf().c_str());
                int c_res = a_res % b_res;
                return new value(NUMBER, std::to_string(c_res));
            }
            if (ast->value() == "equal") {
                auto a = eval(ast->at(0), variables);
                auto b = eval(ast->at(1), variables);
//                std::cout << a->typeOf() << ", " << a->valueOf() << std::endl;
//                std::cout << b->typeOf() << ", " << b->valueOf() << std::endl;
//                if (a->typeOf() != NUMBER || b->typeOf() != NUMBER) {
//                    return new value(UNDEFINED, "undefined");
//                }
//                std::cout << a->valueOf() << std::endl;
                return new value(NUMBER, std::to_string(a->valueOf() == b->valueOf()));
            }
        }
    } else {
//        std::cout << ast->value() << std::endl;
//        std::cout << type(ast->value()) << std::endl;
        if (type(ast->value()) == TOKEN_NAME) {
            return new value(variables.of(ast->value())->valueOf());
        }
        return new value(ast->value());
    }
}