#include <iostream>
#include <cmath>
#include <fstream>
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
    set("undefined", new Undefined);
    set("null", new Null);

    while (true) {
        if (!child) ast->parse();
//        if (!ast->rValue()) continue;
        if (ast->value() == "PROGRAM-END") break;
        if (ast->value() == "bad-tree") {
            std::cerr << ast->rValue()->get(0)->value() << " at <" << filename << ":" << upscope << ">:" << ast->line() << std::endl;
            THROW;
        } else if (ast->value() == "export") {
            auto name = ast->rValue()->get(0)->value();
            auto var = scope->get(name);
            NOTDEFINED(var, name);
            exports->set(name, var->value());
        } else if (ast->value() == "pass") {
            // 因为是pass所以直接跳过即可
        } else if (ast->value() == "import") {
            Interpreter nameIp("", filename, this);
            nameIp.child = true;
            nameIp.ast->root = ast->rValue()->get(0);
            nameIp.child = true;
            Object* e = nameIp.run();
            CHECKITER(e, ast);
            Object *nameRaw = e->get(PASS_RETURN);
            if (nameRaw->type() == STRING) {
                // 基础信息
                string name(((String *) nameRaw)->value());
                string asName(ast->rValue()->get(1)->get(0)->value());
                import(exports, name, asName);
            } WRONGSCRIPT("import");
        } else if (ast->value() == "let") {
            auto name = ast->rValue()->get(0)->value();
            Interpreter ip("", filename, this);
            ip.ast->root = ast->rValue()->get(1);
            ip.child = true;
            auto e = ip.run();
            CHECKITER(e, ip.ast);
            scope->set(name, e->get(PASS_RETURN));
        } else if (ast->value() == "if") {
            auto conAst = ast->rValue()->get(0);
            Interpreter conIp("", filename, this);
            conIp.child = true;
            conIp.ast->root = conAst;
            auto conE = conIp.run();
            CHECKITER(conE, conIp.ast);
            auto con = conE->get(PASS_RETURN);
            if (isTrue(con)) {
                string script(ast->rValue()->get(1)->value());
                Interpreter scriptIp(script, filename, this);
                auto e = scriptIp.run();
                CHECKITER(e, scriptIp.ast);
                auto pb = (Number*) e->get(PASS_BREAK);
                if (pb) exports->set(PASS_BREAK, new Number(pb->value()));
                delete e;
            } else {
                auto els = ast->rValue()->get(2);
                for (UL i = 0; i < els->length(); i++) {
                    auto elconAst = els->get(i);
                    if (elconAst->value() == "elif") {
                        Interpreter elconIp("", filename, this);
                        elconIp.child = true;
                        elconIp.ast->root = elconAst->get(0);
                        auto elconE = elconIp.run();
                        CHECKITER(conE, conIp.ast);
                        auto elcon = elconE->get(PASS_RETURN);
                        if (isTrue(elcon)) {
                            string script(elconAst->get(1)->value());
                            Interpreter scriptIp(script, filename, this);
                            auto e = scriptIp.run();
                            CHECKITER(e, scriptIp.ast);
                            auto pb = (Number*) e->get(PASS_BREAK);
                            if (pb) exports->set(PASS_BREAK, new Number(pb->value()));
                            delete e;
                            break;
                        }
                    } else {
                        string script(elconAst->get(0)->value());
                        Interpreter scriptIp(script, filename, this);
                        auto e = scriptIp.run();
                        CHECKITER(e, scriptIp.ast);
                        auto pb = (Number*) e->get(PASS_BREAK);
                        if (pb) exports->set(PASS_BREAK, new Number(pb->value()));
                        delete e;
                        break;
                    }
                }
            }
            delete conE;
        } else if (ast->value() == "for") {
            // 初始化
            auto initAst = ast->rValue()->get(0);
            Interpreter initIp("", filename, this);
            initIp.ast->root = initAst;
            initIp.child = true;
            auto initE = initIp.run();
            CHECKITER(initE, initAst);
            scope->load(initIp.scope);

            // 获取条件
            auto conAst = ast->rValue()->get(1);
            auto nxtAst = ast->rValue()->get(2);
            string script(ast->rValue()->get(3)->value());// 获取for循环代码块
            while (true) {
                // 条件判断
                Interpreter conIp("", filename, this);
                conIp.ast->root = conAst;
                conIp.child = true;
                auto conE = conIp.run();
                CHECKITER(conE, conAst);
                if (!isTrue(conE->get(PASS_RETURN))) break;

                // 代码块运行
                Interpreter scriptIp(script, filename, this);
                scriptIp.scope->load(conIp.scope);
                auto scriptE = scriptIp.run();
                CHECKITER(scriptE, scriptIp.ast);
                auto pb = (Number*) scriptE->get(PASS_BREAK);
                if (pb) {
                    double v = pb->value() - 1;
                    if (v < 1) break;
                    set(PASS_BREAK, new Number(v));
                    break;
                }

                // 最后表达式执行
                Interpreter nxtIp("", filename, this);
                nxtIp.ast->root = nxtAst;
                nxtIp.child = true;
                auto nxtE = nxtIp.run();
                CHECKITER(nxtE, nxtAst);
            }
        } else if (ast->value() == "while") {
            auto conAst = ast->rValue()->get(0);
            auto script = ast->rValue()->get(1)->value();
            while (true) {
                Interpreter conIp("", filename, this);
                conIp.ast->root = conAst;
                conIp.child = true;
                auto conE = conIp.run();
                CHECKITER(conE, conAst);
                auto con = conE->get(PASS_RETURN);
                if (isTrue(con)) {
                    Interpreter ip(script, filename, this);
                    auto e = ip.run();
                    CHECKITER(e, ast->rValue());
                    auto pb = (Number*) e->get(PASS_BREAK);
                    if (pb) {
                        double v = pb->value() - 1;
                        if (v < 1) break;
                        set(PASS_BREAK, new Number(v));
                        break;
                    }
                } else break;
            }
        } else if (ast->value() == "do") {
            auto script = ast->rValue()->get(0)->value();
            auto conAst = ast->rValue()->get(1);
            while (true) {
                Interpreter ip(script, filename, this);
                auto e = ip.run();
                CHECKITER(e, ast->rValue());
                auto pb = (Number*) e->get(PASS_BREAK);
                if (pb) {
                    double v = pb->value() - 1;
                    if (v < 1) break;
                    set(PASS_BREAK, new Number(v));
                    break;
                }

                Interpreter conIp("", filename, this);
                conIp.ast->root = conAst;
                conIp.child = true;
                auto conE = conIp.run();
                CHECKITER(conE, conAst);
                auto con = conE->get(PASS_RETURN);
                if (!isTrue(con))
                    break;
            }
        } else if (ast->value() == "break") {
            auto count = atoi(ast->rValue()->get(0)->value().c_str());
            exports->set(PASS_BREAK, new Number(count));
        } else if (ast->value() == "continue") {
            exports->set(PASS_CONTINUE, new Number);
            break;
        } else if (ast->value() == "using") {
            auto objAst = ast->rValue()->get(0);
            Using(exports, objAst);
        } else if (ast->value() == "def") {
            string funname(ast->rValue()->get(0)->value());
            string script(ast->rValue()->get(2)->value());
            auto fun = new Function(funname, script);
            auto args = ast->rValue()->get(1);
            for (UL i = 0; i < args->length(); i++) {
                auto arg = args->get(i);
                string argname(arg->value());
                string defaultValue = arg->get(1)->value();
                fun->addDesc(argname);
                Interpreter tmp(defaultValue, filename, this);
                auto tmpe = tmp.run();
                CHECKITER(tmpe, arg);
                fun->defaultValue(argname, tmpe->get(PASS_RETURN));
            }
            set(funname, fun);
            exports->set(PASS_RETURN, fun);
        } else if (ast->value() == "class") {
            string className(ast->rValue()->get(0)->value());
            set(className, new Object);
            auto prototype = new Object;
            get(className)->value()->set("prototype", prototype);
            auto prototypes = ast->rValue()->get(0);
            for (UL i = 0; i < prototypes->length(); i++) {
                auto prototypeNode = prototypes->get(i);
                string name(prototypeNode->value());
                bool pflag(true);// private flag
                if (prototypeNode->get(0)->value() == "public") pflag = false;
                Interpreter ip("", filename, this);
                ip.ast->root = prototypeNode->get(1);
                ip.child = true;
                auto e = ip.run();
                CHECKITER(e, prototypeNode->get(1));
                prototype->set(name, e->get(PASS_RETURN));
            }
        } else if (ast->value() == "new") {
            auto line = ast->rValue()->get(0)->line();
            string name(ast->rValue()->get(0)->value());
            ast->rValue()->get(0)->value(".");
            ast->rValue()->get(0)->insert(name, line);
            ast->rValue()->get(0)->insert("ctor", line);
            ast->rValue()->value("call");
            ast;
        }
        else { //为表达式
            auto len = ast->rValue()->length();
            if (ast->value() == "call") {
                auto r = ast->rValue();
                auto argsNode = r->get(1);
                vector<Object *> args;
                map<string, Object *> hashArg;
                for (UL i = 0; i < argsNode->length(); i++) {
                    auto node = argsNode->get(i);
                    if (node->value() == "=") {
                        auto argName = node->get(0)->value();
                        if (node->get(0)->length() > 0) {
                            std::cerr << "ReferenceError: Invalid setting with " << argName << " at <" << filename
                                      << ":" << upscope << ">:"
                                      << ast->line() << std::endl;
                            THROW;
                        }
                        auto valueAst = node->get(1);
                        Interpreter ip("", filename, this);
                        ip.child = true;
                        ip.ast->root = valueAst;
                        auto e = ip.run();
                        CHECKITER(e, ast);
                        hashArg.insert(std::pair<string, Object *>(argName, e->get(PASS_RETURN)));
                    } else {
                        auto valueAst = node;
                        Interpreter ip("", filename, this);
                        ip.child = true;
                        ip.ast->root = valueAst;
                        auto e = ip.run();
                        CHECKITER(e, ast);
                        args.push_back(e->get(PASS_RETURN));
                    }
                }
                Interpreter getIp("", filename, this);
                getIp.child = true;
                getIp.ast->root = r->get(0);
                auto e = getIp.run();
                auto fun_ = e->get(PASS_RETURN);
                auto up = e->get(PASS_UPVALUE);
                bool clean = false;
                if (!up) {
                    up = new Undefined;
                    clean = true;
                }
                if (!fun_) {
                    std::cerr << "TypeError: The value for getting is not defined at <" << filename << ":" << upscope << ">:"
                              << ast->line() << std::endl;
                    THROW;
                }
                if (fun_->type() == FUNCTION) {
                    auto fun = (Function *) fun_;
                    fun->setParent(this);
                    fun->getParent()->set("this", up);
                    exports->set(PASS_RETURN, fun->run(args, hashArg));
                } else if (fun_->type() == NATIVE_FUNCTION) {
                    auto fun = (NativeFunction *) fun_;
                    fun->setParent(this);
                    fun->getParent()->set("this", up);
                    exports->set(PASS_RETURN, fun->run(args, hashArg));
                } else {
                    if (clean) delete up;
                    std::cerr << "TypeError: The value for getting is not a function at <" << filename << ":" << upscope << ">:"
                              << ast->line() << std::endl;
                    THROW;
                }
                if (clean) delete up;
                del("this");
            } else if (ast->value() == "get") {
                if (ast->rValue()->length() < 1) {
                    auto name = ast->value();
                    auto var = scope->get(name);
                    NOTDEFINED(var, name);
                    exports->set(PASS_RETURN, var->value());
                } else {
                    string name(ast->rValue()->get(0)->value());
                    auto v = scope->get(name);
                    NOTDEFINED(v, name);
                    BM::Object* value = v->value();
                    BM::Object* up = nullptr;
                    string key;
                    for (UL i = 1; i < ast->rValue()->length(); i++) {// 下标为0的是v的名字
                        Interpreter ip("", filename, this);
                        ip.ast->root = ast->rValue()->get(i);
                        ip.child = true;
                        auto e = ip.run();
                        CHECKITER(e, ast);
                        key = e->get(PASS_RETURN)->toString(false, false);
                        up = value;
                        value = value->get(key);
                        if (!value) {
                            if (i == ast->rValue()->length() - 1) {
                                value = new Undefined;
                                up->set(key, value);
                            } else {
                                std::cerr << "ReferenceError: Cannot get property "
                                          << e->get(PASS_RETURN)->toString(false, false) << ", it is not defined at <"
                                          << filename << ":" << upscope << ">:"
                                          << ast->line() << std::endl;
                                THROW;
                            }
                        }
                    }
                    exports->set(PASS_RETURN, value);
                    exports->set(PASS_UPVALUE, up);
                    exports->set(PASS_LASTKEY, new String(key));
                }
            } else if (ast->value() == ".") {
                string startV(ast->rValue()->get(0)->value());
                vector<string> keys;
                auto node = ast->rValue()->get(1);
                while (true) {
                    if (node->length() < 1) {
                        keys.push_back(node->value());
                        break;
                    } else {
                        keys.push_back(node->get(0)->value());
                    }
                    node = node->get(1);
                }
                auto var = scope->get(startV);
                if (var) {
                    Object* up = nullptr;
                    auto value = var->value();
                    for (UL i = 0; i < keys.size(); i++) {
                        auto last = value;
                        up = value;
                        value = value->get(keys[i]);
                        if (!value) {
                            if (i == keys.size() - 1) {
                                last->set(keys[i], new Undefined);
                                value = last->get(keys[i]);
                            } WRONG("ReferenceError", keys[i] + " is not defined in " + startV + " or its properties");
                        }
                    }
                    exports->set(PASS_RETURN, value);
                    exports->set(PASS_UPVALUE, up);
                    exports->set(PASS_LASTKEY, new String(keys[keys.size() - 1]));
                } WRONG("ReferenceError", startV + " is not defined");
            } else if (ast->value() == "o-value") {
                auto object = new Object;
                for (L i = 0; i < ast->rValue()->length(); i++) {
                    string key(ast->rValue()->get(i)->value());
                    auto valueNode = ast->rValue()->get(i)->get(0);
                    string valueStr(valueNode->value());
                    Interpreter ip(valueStr, filename, this);
                    auto e = ip.run();
                    CHECKITER(e, valueNode);
                    object->set(key, e->get(PASS_RETURN));
                }
                exports->set(PASS_RETURN, object);
            } else if (len < 1) {
                if (isNumber(ast->value())) {
                    exports->set(PASS_RETURN, new Number(transSD(ast->value())));
                } else if (isString(ast->value())) {
                    auto s = ast->value();
                    s.erase(0, 1);
                    s.erase(s.length() - 1, 1);
                    exports->set(PASS_RETURN, (new String(s))->trans());
                } else {
                    auto name = ast->value();
                    auto var = scope->get(name);
                    NOTDEFINED(var, name);
                    exports->set(PASS_RETURN, var->value());
                }
            } else if (len == 1) {
                if (ast->value() == "++" || ast->value() == "--") {
                    Interpreter ip("", filename, this);
                    ip.ast->root = ast->rValue()->get(0);
                    ip.child = true;
                    auto e = ip.run();
                    CHECKITER(e, ast);
                    auto n = e->get(PASS_RETURN);
//                std::cout << n->type() << std::endl;
                    if (ast->value() == "++") {
                        if (n->type() == NUMBER) {
                            Number *v = (Number *) n;
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
                    auto valueIp = new Interpreter("", filename, this);
                    valueIp->ast->root = valueAst;
                    valueIp->child = true;
                    auto e_ = valueIp->run()->get(PASS_RETURN);
                    delete valueIp;

                    auto op = ast->value();
                    if (e_->type() == NUMBER) {
                        auto e = (Number *) e_;
                        double &s = e->value();
                        if (op == "!") {
                            s = (bool) s;
                        } else if (op == "~") {
                            s = ~((LL) s);
                        }
                        exports->set(PASS_RETURN, e);
                    } WRONGEXPRTYPE(op);
                }
            } else if (len == 2) {
                auto leftAst = ast->rValue()->get(0);
                auto rightAst = ast->rValue()->get(1);
                auto leftIp = new Interpreter("", filename, this);
                auto rightIp = new Interpreter("", filename, this);
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
                    } else if (left->type() == STRING && right->type() == STRING) {
                        auto leftV = ((String *) (left))->value();
                        auto rightV = ((String *) (right))->value();
                        exports->set(PASS_RETURN, (new String(leftV + rightV))->trans());
                    } WRONGEXPRTYPE(op);
                } else if (
                        op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "**=" ||
                        op == "<<=" || op == ">>=" || op == "|=" || op == "&=" || op == "^="
                        ) {
                    auto leftNode = ast->rValue()->get(0);
                    string name(leftNode->value());
                    Object* up;
                    string key;
                    if (name == "." || (name == "get" && leftNode->length() > 0)) {
                        Interpreter ip("", filename, this);
                        ip.child = true;
                        ip.ast->root = leftNode;
                        auto e = ip.run();
                        CHECKITER(e, leftNode);
                        up = e->get(PASS_UPVALUE);
                        key = ((String*)e->get(PASS_LASTKEY))->value();
                        if (op == "=") up->set(key, right);
                    } else if (leftNode->length() > 0 || isNumber(name) || isString(name)) {
                        std::cerr << "ReferenceError: Invalid left-hand side in assignment at <" << filename << ":" << upscope << ">:"
                                  << ast->line() << std::endl;
                        THROW;
                    }
                    if (op == "=" && !(name == "." || (name == "get" && leftNode->length() > 0))) scope->set(name, right);
                    else {
                        Object* value_ = nullptr;
                        if (name == "." || (name == "get" && leftNode->length() > 0)) {
                            value_ = up->get(key);
                        } else {
                            auto var = scope->get(name);
                            value_ = var->value();
                        }
                        if (op == "+=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() += ((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } else if (value_->type() == STRING) {
                                if (right->type() == STRING) {
                                    String *value = (String *) value_;
                                    value->value() += ((String *) right)->value();
                                    value->trans();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "-=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() -= ((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "*=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() *= ((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "/=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() /= ((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "%=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = (int)(value->value()) % (int)((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "**=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = pow(value->value(), ((Number *) right)->value());
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "<<=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = (int)(value->value()) << (int)((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == ">>=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = (int)(value->value()) >> (int)((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "|=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = (int)(value->value()) | (int)((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "&=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = (int)(value->value()) & (int)((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        } else if (op == "^=") {
                            if (value_->type() == NUMBER) {
                                if (right->type() == NUMBER) {
                                    Number *value = (Number *) value_;
                                    value->value() = (int)(value->value()) ^ (int)((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } WRONGEXPRTYPE(op);
                        }
                    }
                } else if (
                        op == "==" || op == "<=" || op == ">=" || op == "<" || op == ">" || op == "!="
                        ) {
                    if (left->type() != right->type()) {
                        std::cerr << "TypeError" << ": " << "Cannot compare values with two different types" << " at <" << filename << ":" << upscope << ">:" << ast->line() << std::endl;
                        THROW;
                    }
                    switch (left->type()) {
                        case NUMBER:
                        {
                            auto leftV = ((Number *) (left))->value();
                            auto rightV = ((Number *) (right))->value();
                            if (op == "==") exports->set(PASS_RETURN, new Number(leftV == rightV));
                            else if (op == "<=") exports->set(PASS_RETURN, new Number(leftV <= rightV));
                            else if (op == ">=") exports->set(PASS_RETURN, new Number(leftV >= rightV));
                            else if (op == "<") exports->set(PASS_RETURN, new Number(leftV < rightV));
                            else if (op == ">") exports->set(PASS_RETURN, new Number(leftV > rightV));
                            else if (op == "!=") exports->set(PASS_RETURN, new Number(leftV != rightV));
                            break;
                        }
                        case STRING:
                        {
                            auto leftV = ((String *) (left))->value();
                            auto rightV = ((String *) (right))->value();
                            if (op == "==") exports->set(PASS_RETURN, new Number(leftV == rightV));
                            else if (op == "<=") exports->set(PASS_RETURN, new Number(leftV <= rightV));
                            else if (op == ">=") exports->set(PASS_RETURN, new Number(leftV >= rightV));
                            else if (op == "<") exports->set(PASS_RETURN, new Number(leftV < rightV));
                            else if (op == ">") exports->set(PASS_RETURN, new Number(leftV > rightV));
                            else if (op == "!=") exports->set(PASS_RETURN, new Number(leftV != rightV));
                            break;
                        }
                        case NULL_:
                        case UNDEFINED:
                        {
                            if (op == "==") exports->set(PASS_RETURN, new Number(1));
                            else if (op == "<=") exports->set(PASS_RETURN, new Number(0));
                            else if (op == ">=") exports->set(PASS_RETURN, new Number(0));
                            else if (op == "<") exports->set(PASS_RETURN, new Number(0));
                            else if (op == ">") exports->set(PASS_RETURN, new Number(0));
                            else if (op == "!=") exports->set(PASS_RETURN, new Number(0));
                            break;
                        }
                        default:
                        {
                            auto leftV = (long long)left;
                            auto rightV = (long long)right;
                            if (op == "==") exports->set(PASS_RETURN, new Number(leftV == rightV));
                            else if (op == "<=") exports->set(PASS_RETURN, new Number(0));
                            else if (op == ">=") exports->set(PASS_RETURN, new Number(0));
                            else if (op == "<") exports->set(PASS_RETURN, new Number(0));
                            else if (op == ">") exports->set(PASS_RETURN, new Number(0));
                            else if (op == "!=") exports->set(PASS_RETURN, new Number(leftV != rightV));
                            break;
                        }
                    }
                } else {
                    RIGHTEXPRTYPE(left, right) {
                        auto leftV = ((Number *) (left))->value();
                        auto rightV = ((Number *) (right))->value();
                        if (op == "-") exports->set(PASS_RETURN, new Number(leftV - rightV));
                        else if (op == "*") exports->set(PASS_RETURN, new Number(leftV * rightV));
                        else if (op == "/") exports->set(PASS_RETURN, new Number(leftV / rightV));
                        else if (op == "%") exports->set(PASS_RETURN, new Number(((LL) (leftV)) % ((LL) (rightV))));
                        else if (op == "**") exports->set(PASS_RETURN, new Number(pow(leftV, rightV)));
                        else if (op == "&") exports->set(PASS_RETURN, new Number(((LL) (leftV)) & ((LL) (rightV))));
                        else if (op == "|") exports->set(PASS_RETURN, new Number(((LL) (leftV)) | ((LL) (rightV))));
                        else if (op == "^") exports->set(PASS_RETURN, new Number(((LL) (leftV)) ^ ((LL) (rightV))));
                        else if (op == "||") exports->set(PASS_RETURN, new Number(((bool) (leftV)) || ((bool) (rightV))));
                        else if (op == "&&") exports->set(PASS_RETURN, new Number(((bool) (leftV)) && ((bool) (rightV))));
                        else if (op == "<<") exports->set(PASS_RETURN, new Number(((LL) (leftV)) << ((LL) (rightV))));
                        else if (op == ">>") exports->set(PASS_RETURN, new Number(((LL) (leftV)) >> ((LL) (rightV))));
                    } WRONGEXPRTYPE(op);
                }
            }
        }
        if (child) break;
    }
    if (!exports->get(PASS_RETURN)) exports->set(string(PASS_RETURN), new Undefined);
    return exports;
}

BM::Object *BM::Interpreter::runCC() {
    ast->importByString(script);
    return run();
}

void BM::Interpreter::import(Object* exports, const string& name, const string& asName) {
    UL nameLen = name.length();
    bool finish = false;
    string tmpLine;
    string script;

    std::ifstream file;
    // 为本地文件
    if (name[nameLen - 3] == '.' && name[nameLen - 2] == 'b' && name[nameLen - 1] == 'm') {
        file.open(name);
        if (file.is_open()) {
            finish = true;
            while (getline(file, tmpLine)) {
                script += tmpLine + "\n";
            }
            Interpreter ip(script, name);// import的文件是独立运行的，与import它的脚本连接
            auto moduleExports = ip.run();
            if (moduleExports->get(PASS_ERROR)) {
                std::cerr << "ImportError: Module script wrong at <" << filename << ">:" << ast->line() << std::endl;
                exports->set(PASS_ERROR, new Number(1));
            }
            moduleExports->del(PASS_RETURN);
            scope->set(asName, moduleExports);
        }
    }
    // 为本地模块
    if (!finish) {
        string path(name + (name[nameLen - 1] == '/' ? "init.bm" : "/init.bm"));
        file.open(path);
        if (file.is_open()) {
            finish = true;
            while (getline(file, tmpLine)) {
                script += tmpLine + "\n";
            }
            Interpreter ip(script, name);// import的文件是独立运行的，与import它的脚本连接
            auto moduleExports = ip.run();
            if (moduleExports->get(PASS_ERROR)) {
                std::cerr << "ImportError: Module script wrong at <" << filename << ">:" << ast->line() << std::endl;
                exports->set(PASS_ERROR, new Number(1));
            }
            moduleExports->del(PASS_RETURN);
            scope->set(asName, moduleExports);
        }
    }
    // 为全局模块
    if (!finish) {
        string path(BMLMPATH + name + (name[nameLen - 1] == '/' ? "init.bm" : "/init.bm"));
        file.open(name);
        if (file) {
            finish = true;
            while (getline(file, tmpLine)) {
                script += tmpLine + "\n";
            }
            Interpreter ip(script, name);// import的文件是独立运行的，不与import它的脚本连接
            auto moduleExports = ip.run();
            if (moduleExports->get(PASS_ERROR)) {
                std::cerr << "ImportError: Module script wrong at <" << filename << ">:" << ast->line() << std::endl;
                exports->set(PASS_ERROR, new Number(1));
            }
            moduleExports->del(PASS_RETURN);
            scope->set(asName, moduleExports);
        }
    }

    // 为拓展库
    if (!finish) {
        string path(BMMPATH + name);
        Dylib dylib(path);
        if (dylib.load()) {
            auto initModule = (initModuleFun) dylib.resolve("initModule");
            Object *moduleExports = initModule();
            scope->set(asName, moduleExports);
        } else std::cerr << "ImportError" << ": No module named " << name << " at <" << filename <<  ":" << upscope << ">:" << ast->line() << std::endl;
        dylib.close();
    }
}
void BM::Interpreter::Using(Object* exports, AST::node* objAst) {
    Interpreter ip("", filename, this);
    ip.ast->root = objAst;
    ip.child = true;
    auto e = ip.run();
    if (!e || e->get(PASS_ERROR)) std::cerr << "at <" << filename << ":" << upscope << ">:" << ast->line() << std::endl;
    auto obj = e->get(PASS_RETURN);
    Object::Iterator iter(obj);
    for (; !iter.end(); iter.next()) {
        set(iter.key(), iter.value());
    }
}