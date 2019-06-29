#include <iostream>
#include <cmath>
#include <cstdlib>
#include "value.h"
#include "dylib.h"
#include "ast.h"
#include "interpreter.h"

string BM::Interpreter::compile() {
    string res;
    while (true) {
        ast->parse();
        if (ast->value() == "PROGRAM-END") break;
        res += ast->exportByString();
    }
    return res;
}

BM::Object *BM::Interpreter::run() {
    Object *exports = new Object;
    scope->set(SCOPE_D_NAME, new Object);
    while (true) {
        if (!child) ast->parse();
        if (ast->value() == "PROGRAM-END") break;
        if (ast->value() == "bad-tree") {
            std::cerr << ast->rValue()->get(0)->value() << "at <" << filename << ">:" << ast->line() << std::endl;
            THROW;
        } else if (ast->value() == "export") {
            auto name = ast->rValue()->get(0)->value();
            auto var = scope->get(name);
            if (!var) {
                std::cerr << "ReferenceError: " << name << " is not defined at <" << filename << ">:"
                          << ast->line() << std::endl;
                THROW;
            }
            exports->set(name, var->value());
        } else if (ast->value() == "import") {
            Interpreter nameIp("", filename);
            nameIp.child = true;
            nameIp.ast->root = ast->rValue()->get(0);
            auto e = nameIp.run();
            Object* nameRaw = e->get(PASS_RETURN);
            if (nameRaw->type() == STRING) {
                string name(((String*)nameRaw)->value());
                string asName(ast->rValue()->get(1)->get(0)->value());
                string path(BMMPATH + name);
                Dylib dylib(path);
                if (dylib.load()) {
                    auto initModule = (initModuleFun) dylib.resolve("initModule");
                    Object* moduleExports = initModule();
                    scope->set(asName, moduleExports);
                } WRONG("ImportError", "No module named " + name);
                dylib.close();
            } WRONGSCRIPT("import");
        } else if (ast->value() == "let") {
            auto name = ast->rValue()->get(0)->value();
            Interpreter ip("", filename);
            ip.ast->root = ast->rValue()->get(1);
            ip.child = true;
            auto e = ip.run();
            CHECKITER(e, ip.ast);
            scope->set(name, e->get(PASS_RETURN));
        } else { //为表达式
            auto len = ast->rValue()->length();
            if (ast->value() == "call") {
                auto r = ast->rValue();
                auto argsNode = r->get(1);
                string name(r->get(0)->value());
                vector<Object*> args;
                map<string, Object*> hashArg;
                for (UL i = 0; i < argsNode->length(); i++) {
                    auto node = argsNode->get(i);
                    if (node->value() == "=") {
                        auto argName = node->get(0)->value();
                        if (node->get(0)->length() > 0 || isNumber(name) || isString(name)) {
                            std::cerr << "ReferenceError: Invalid setting with " << argName << " at <" << filename
                                      << ">:"
                                      << ast->line() << std::endl;
                            THROW;
                        }
                        auto valueAst = node->get(1);
                        Interpreter ip("", filename);
                        ip.child = true;
                        ip.ast->root = valueAst;
                        auto e = ip.run();
                        hashArg.insert(std::pair<string, Object*>(argName, e->get(PASS_RETURN)));
                    } else {
                        auto valueAst = node;
                        Interpreter ip("", filename);
                        ip.child = true;
                        ip.ast->root = valueAst;
                        auto e = ip.run();
                        args.push_back(e->get(PASS_RETURN));
                    }
                }
                auto fun_ = scope->get(name);
                if (!fun_) {
                    std::cerr << "ReferenceError: " << name << " is not defined at <" << filename << ">:"
                              << ast->line() << std::endl;
                    THROW;
                }
                if (fun_->value()->type() == FUNCTION) {
                    auto fun = (Function*)fun_;
                    exports->set(PASS_RETURN, fun->run(args, hashArg));
                } else if (fun_->value()->type() == NATIVE_FUNCTION) {
                    auto fun = (NativeFunction*)fun_;
                    exports->set(PASS_RETURN, fun->run(args, hashArg));
                } else {
                    std::cerr << "TypeError: " << name << " is not a function at <" << filename << ">:"
                              << ast->line() << std::endl;
                    THROW;
                }
            } else if (ast->value() == "get") {
//                auto v = scope->get(ast->rValue()->get(0)->value());
            } else if (len < 1) {
                if (isNumber(ast->value())) {
                    exports->set(PASS_RETURN, new Number(transSD(ast->value())));
                } else if (isString(ast->value())) {
                    auto s = ast->value();
                    s.erase(0, 1);
                    s.erase(s.length() - 1, 1);
                    exports->set(PASS_RETURN, new String(s));
                } else {
                    auto name = ast->value();
                    auto var = scope->get(name);
                    if (!var) {
                        std::cerr << "ReferenceError: " << name << " is not defined at <" << filename << ">:"
                                  << ast->line() << std::endl;
                        THROW;
                    }
                    exports->set(name, var->value());
                }
            } else if (len == 1) {
                if (ast->value() == "++" || ast->value() == "--") {
                    auto name = ast->rValue()->get(0)->value();
                    if (isNumber(name) || isString(name)) {
                        std::cerr << "ReferenceError: Invalid operator with " << name << " at <" << filename
                                  << ">:"
                                  << ast->line() << std::endl;
                        THROW;
                    }
                    auto var = scope->get(name);
                    if (!var) {
                        std::cerr << "ReferenceError: " << name << " is not defined at <" << filename << ">:"
                                  << ast->line() << std::endl;
                        THROW;
                    }
                    auto n = var->value();
//                std::cout << n->type() << std::endl;
                    if (ast->value() == "++") {
                        if (n->type() == NUMBER) {
                            Number* v = (Number*)n;
                            v->value()++;
                        } WRONGEXPRTYPE(ast->value());
                    } else if (ast->value() == "--") {
                        if (n->type() == NUMBER) {
                            Number *v = (Number *) n;
                            v->value()--;
                        } WRONGEXPRTYPE(ast->value());
                    }
                } else {
                    auto valueAst = ast->rValue()->get(0);
                    auto valueIp = new Interpreter("", filename);
                    valueIp->ast->root = valueAst;
                    valueIp->child = true;
                    auto e_ = valueIp->run()->get(PASS_RETURN);
                    delete valueIp;

                    auto op = ast->value();
                    if (e_->type() == NUMBER) {
                        auto e = (Number*)e_;
                        double& s = e->value();
                        if (op == "!") {
                            s = (bool)s;
                        } else if (op == "~") {
                            s = ~((LL)s);
                        }
                        exports->set(PASS_RETURN, e);
                    } WRONGEXPRTYPE(op);
                }
            } else if (len == 2) {
                auto leftAst = ast->rValue()->get(0);
                auto rightAst = ast->rValue()->get(1);
                auto leftIp = new Interpreter("", filename);
                auto rightIp = new Interpreter("", filename);
                leftIp->ast->root = leftAst;
                rightIp->ast->root = rightAst;
                leftIp->child = true;
                rightIp->child = true;
                auto left = leftIp->run()->get(PASS_RETURN);
                CHECKITER(left, leftIp->ast);
                auto right = rightIp->run()->get(PASS_RETURN);
                CHECKITER(right, rightIp->ast);
                string op(ast->value());
                delete leftIp;
                delete rightIp;
                if (op == "+") {
                    if (left->type() == NUMBER && right->type() == NUMBER) {
                        auto leftV = ((Number *) (left))->value();
                        auto rightV = ((Number *) (right))->value();
                        exports->set(PASS_RETURN, new Number(leftV + rightV));
                    } WRONGEXPRTYPE(op);
                } else if (
                        op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "**=" ||
                        op == "<<=" || op == ">>=" || op == "|=" || op == "&=" || op == "^=" || op == "/="
                        ) {
                    auto leftNode = ast->rValue()->get(0);
                    string name(leftNode->value());
                    if (leftNode->length() > 0 || isNumber(name) || isString(name)) {
                        std::cerr << "ReferenceError: Invalid left-hand side in assignment at <" << filename << ">:"
                                  << ast->line() << std::endl;
                        THROW;
                    }
                    if (op == "=") scope->set(name, right);
                    else {
                        auto var = scope->get(name);
                        auto value_ = var->value();
                        if (op == "+=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number* value = (Number*)value_;
                                    value->value() += ((Number*)right)->value();
                                } WRONGEXPRTYPE(op);
                            } else if (value_->type() == STRING) {
                                if (right->type() == STRING) {
                                    String* value = (String*)value_;
                                    value->value() += ((String*)right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        }
                    }
                } else {
                    RIGHTEXPRTYPE(left, right) {
                        auto leftV = ((Number *) (left))->value();
                        auto rightV = ((Number *) (right))->value();
                        if (op == "-") exports->set(PASS_RETURN, new Number(leftV - rightV));
                        if (op == "*") exports->set(PASS_RETURN, new Number(leftV * rightV));
                        if (op == "/") exports->set(PASS_RETURN, new Number(leftV / rightV));
                        if (op == "%") exports->set(PASS_RETURN, new Number(((LL)(leftV)) % ((LL)(rightV))));
                        if (op == "**") exports->set(PASS_RETURN, new Number(pow(leftV, rightV)));
                        if (op == "&") exports->set(PASS_RETURN, new Number(((LL)(leftV)) & ((LL)(rightV))));
                        if (op == "|") exports->set(PASS_RETURN, new Number(((LL)(leftV)) | ((LL)(rightV))));
                        if (op == "^") exports->set(PASS_RETURN, new Number(((LL)(leftV)) ^ ((LL)(rightV))));
                        if (op == "||") exports->set(PASS_RETURN, new Number(((bool)(leftV)) || ((bool)(rightV))));
                        if (op == "&&") exports->set(PASS_RETURN, new Number(((bool)(leftV)) && ((bool)(rightV))));
                        if (op == "<<") exports->set(PASS_RETURN, new Number(((LL)(leftV)) << ((LL)(rightV))));
                        if (op == ">>") exports->set(PASS_RETURN, new Number(((LL)(leftV)) >> ((LL)(rightV))));
                    } WRONGEXPRTYPE(op);
                }
            }
        }
        if (child) break;
    }
    scope->clear();
    if (!exports->get(PASS_RETURN)) exports->set(string(PASS_RETURN), new Undefined);
    return exports;
}

BM::Object *BM::Interpreter::runCC() {
    ast->importByString(script);
    return run();
}