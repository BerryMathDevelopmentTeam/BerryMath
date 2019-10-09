#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include "value.h"
#include "dylib.h"
#include "ast.h"
#include "types.h"
#include "types.cpp"
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

    if (!child && !loaded && !(get(PASS_MODULE_NAME) && get(PASS_MODULE_NAME)->value()->type() == STRING && ((String*)get(PASS_MODULE_NAME)->value())->value() == DEFAULT_IMPORT_NAME)) {
        if (!get(DEFAULT_IMPORT_NAME)) import(exports, DEFAULT_IMPORT_NAME, DEFAULT_IMPORT_NAME);
        auto objAst = new AST::node("bmlang", 0);
        Using(get(DEFAULT_IMPORT_NAME)->value(), objAst);
        set("undefined", new Undefined);
        set("null", new Null);
        loaded = true;
        delete objAst;
    }
    if (!ast) ast = new AST(script);
    while (true) {
        if (!child) ast->parse();
        string rootValue = ast->value();
//        if (!ast->rValue()) continue;
        if (rootValue == "PROGRAM-END") break;
        if (rootValue == "bad-tree") {
            std::clog << ast->rValue()->get(0)->value() << "\n\tat <" << filename << ":" << upscope << ">:" << ast->line() << std::endl;
            if (ast) { delete ast;ast = nullptr; }
            THROW;
        } else if (rootValue == "export") {
            auto name = ast->rValue()->get(0)->value();
            auto var = scope->get(name);
            NOTDEFINED(var, name);
            exports->set(name, var->value());
        } else if (rootValue == "pass") {
            // 因为是pass所以直接跳过即可
        } else if (rootValue == "import") {
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
            FREE(e);
        } else if (rootValue == "let") {
            auto name = ast->rValue()->get(0)->value();
            Interpreter ip("", filename, this);
            ip.ast->root = ast->rValue()->get(1);
            ip.child = true;
            auto e = ip.run();
            CHECKITER(e, ip.ast);
            auto ret = e->get(PASS_RETURN);
            if (ret->type() == OBJECT) {
                scope->set(name, ret);
            } else {
                scope->set(name, ret->copy());
                CHECKPASSNEXTOP(e);
                FREE(ret);
            }
        } else if (rootValue == "if") {
            auto conAst = ast->rValue()->get(0);
            Interpreter conIp("", filename, this);
            conIp.child = true;
            conIp.ast->root = conAst;
            auto conE = conIp.run();
            CHECKITER(conE, conIp.ast);
            Object* con;
            OPERPASSNEXTOP(conE, con);
            if (isTrue(con)) {
                string conscript(ast->rValue()->get(1)->value());
                Interpreter scriptIp(conscript, filename, this);
                auto e = scriptIp.run();
                CHECKITER(e, scriptIp.ast);
                CHECKPASSNEXTOP(e);
                auto pb = (Number*) e->get(PASS_BREAK);
                if (pb) exports->set(PASS_BREAK, new Number(pb->value()));
                auto ret = e->get(PASS_ENDFUN);
                if (ret) {
                    exports->set(PASS_ENDFUN, ret);
                    return exports;
                    FREE(e);
                }
                FREE(e);
            } else {
                auto els = ast->rValue()->get(2);
                for (UL i = 0; i < els->length(); i++) {
                    auto elconAst = els->get(i);
                    if (elconAst->value() == "elif") {
                        Interpreter elconIp("", filename, this);
                        elconIp.child = true;
                        elconIp.ast->root = elconAst->get(0);
                        auto elconE = elconIp.run();
                        CHECKITER(elconE, elconIp.ast);
                        Object* elcon;
                        OPERPASSNEXTOP(elconE, elcon);
                        if (isTrue(elcon)) {
                            string conscript(elconAst->get(1)->value());
                            Interpreter scriptIp(conscript, filename, this);
                            auto e = scriptIp.run();
                            CHECKITER(e, scriptIp.ast);
                            CHECKPASSNEXTOP(e);
                            auto pb = (Number*) e->get(PASS_BREAK);
                            if (pb) exports->set(PASS_BREAK, new Number(pb->value()));
                            auto ret = e->get(PASS_ENDFUN);
                            if (ret) {
                                exports->set(PASS_ENDFUN, ret);
                                return exports;
                                FREE(e);
                            }
                            delete e;
                            delete elconE;
                            break;
                        }
                    } else {
                        string elscript(elconAst->get(0)->value());
                        Interpreter scriptIp(elscript, filename, this);
                        auto e = scriptIp.run();
                        CHECKITER(e, scriptIp.ast);
                        CHECKPASSNEXTOP(e);
                        auto pb = (Number*) e->get(PASS_BREAK);
                        if (pb) exports->set(PASS_BREAK, new Number(pb->value()));
                        auto ret = e->get(PASS_ENDFUN);
                        if (ret) {
                            exports->set(PASS_ENDFUN, ret);
                            return exports;
                            FREE(e);
                        }
                        delete e;
                        break;
                    }
                }
            }
            delete conE;
        } else if (rootValue == "for") {
            // 初始化
            auto initAst = ast->rValue()->get(0);
            Interpreter initIp("", filename, this);
            initIp.ast->root = initAst;
            initIp.child = true;
            auto initE = initIp.run();
            CHECKITER(initE, initAst);
            CHECKPASSNEXTOP(initE);
            scope->load(initIp.scope);

            // 获取条件
            auto conAst = ast->rValue()->get(1);
            auto nxtAst = ast->rValue()->get(2);
            string forscript(ast->rValue()->get(3)->value());// 获取for循环代码块
            while (true) {
                // 条件判断
                Interpreter conIp("", filename, this);
                conIp.ast->root = conAst;
                conIp.child = true;
                auto conE = conIp.run();
                CHECKITER(conE, conAst);
                CHECKPASSNEXTOP(conE);
                if (!isTrue(conE->get(PASS_RETURN))) break;

                // 代码块运行
                Interpreter scriptIp(forscript, filename, this);
                scriptIp.scope->load(conIp.scope);
                Object* scriptE = scriptIp.run();
                CHECKITER(scriptE, scriptIp.ast);
                { CHECKPASSNEXTOP(scriptE); }
                auto pb = (Number*) scriptE->get(PASS_BREAK);
                if (pb) {
                    double v = pb->value() - 1;
                    if (v < 1) break;
                    exports->set(PASS_BREAK, new Number(v));
                    break;
                }
                auto ret = scriptE->get(PASS_ENDFUN);
                if (ret) {
                    exports->set(PASS_ENDFUN, ret);
                    return exports;
                    FREE(scriptE);
                }

                // 最后表达式执行
                Interpreter nxtIp("", filename, this);
                nxtIp.ast->root = nxtAst;
                nxtIp.child = true;
                auto nxtE = nxtIp.run();
                CHECKITER(nxtE, nxtAst);
                { CHECKPASSNEXTOP(nxtE); }
                FREE(conE);
                FREE(nxtE);
            }
        } else if (rootValue == "while") {
            auto conAst = ast->rValue()->get(0);
            auto whscript = ast->rValue()->get(1)->value();
            while (true) {
                Interpreter conIp("", filename, this);
                conIp.ast->root = conAst;
                conIp.child = true;
                auto conE = conIp.run();
                CHECKITER(conE, conAst);
                CHECKPASSNEXTOP(conE);
                auto con = conE->get(PASS_RETURN);
                if (isTrue(con)) {
                    Interpreter ip(whscript, filename, this);
                    auto e = ip.run();
                    CHECKITER(e, ast->rValue());
                    CHECKPASSNEXTOP(e);
                    auto pb = (Number*) e->get(PASS_BREAK);
                    if (pb) {
                        double bv = pb->value() - 1;
                        if (bv < 1) break;
                        exports->set(PASS_BREAK, new Number(bv));
                        break;
                    }
                    auto ret = e->get(PASS_ENDFUN);
                    if (ret) {
                        exports->set(PASS_ENDFUN, ret);
                        return exports;
                        FREE(e);
                    }
                } else break;
                FREE(conE);
            }
        } else if (rootValue == "do") {
            auto doscript = ast->rValue()->get(0)->value();
            auto conAst = ast->rValue()->get(1);
            while (true) {
                Interpreter ip(doscript, filename, this);
                auto e = ip.run();
                CHECKITER(e, ast->rValue());
                CHECKPASSNEXTOP(e);
                auto pb = (Number*) e->get(PASS_BREAK);
                if (pb) {
                    double v = pb->value() - 1;
                    if (v < 1) break;
                    exports->set(PASS_BREAK, new Number(v));
                    break;
                }
                auto ret = e->get(PASS_ENDFUN);
                if (ret) {
                    exports->set(PASS_ENDFUN, ret);
                    return exports;
                    FREE(e);
                }

                Interpreter conIp("", filename, this);
                conIp.ast->root = conAst;
                conIp.child = true;
                auto conE = conIp.run();
                CHECKITER(conE, conAst);
                { CHECKPASSNEXTOP(conE); }
                auto con = conE->get(PASS_RETURN);
                if (!isTrue(con))
                    break;
                FREE(conE);
            }
        } else if (rootValue == "break") {
            auto count = atoi(ast->rValue()->get(0)->value().c_str());
            exports->set(PASS_BREAK, new Number(count));
        } else if (rootValue == "return") {
            Interpreter ip(ast->rValue()->get(0)->value(), filename, this);
            auto e = ip.run();
            CHECKITER(e, ast->rValue());
            exports->set(PASS_ENDFUN, e->get(PASS_RETURN));
            delete e;
            return exports;
        } else if (rootValue == "continue") {
            exports->set(PASS_CONTINUE, new Number);
            break;
        } else if (rootValue == "using") {
            auto objAst = ast->rValue()->get(0);
            Using(exports, objAst);
            ast->rValue()->clear();
        } else if (rootValue == "def") {
            string funname(ast->rValue()->get(0)->value());
            string funscript(ast->rValue()->get(2)->value());
            auto fun = new Function(funname, funscript);
            auto args = ast->rValue()->get(1);
            for (UL i = 0; i < args->length(); i++) {
                auto arg = args->get(i);
                string argname(arg->value());
                string defaultValue = arg->get(1)->value();
                if (defaultValue == "pass") defaultValue = "undefined";
                fun->addDesc(argname);
                if (arg->get(0)->value() == "refer") fun->addRefer(argname);
                Interpreter tmp(defaultValue, filename, this);
                auto tmpe = tmp.run();
                CHECKITER(tmpe, arg);
                CHECKPASSNEXTOP(tmpe);
                fun->defaultValue(argname, tmpe->get(PASS_RETURN));
//                FREE(tmpe);
            }
            set(funname, fun);
            exports->set(PASS_RETURN, fun);
        } else if (rootValue == "class") {
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
                if (name == "ctor") {
                    prototypeNode->get(1)->get(2)->value("let this = { };" + prototypeNode->get(1)->get(2)->value() + ";return this;");
                }
                Interpreter ip("", filename, this);
                ip.ast->root = prototypeNode->get(1);
                ip.child = true;
                auto e = ip.run();
                CHECKITER(e, prototypeNode->get(1));
                CHECKPASSNEXTOP(e);
                prototype->set(name, e->get(PASS_RETURN));
                FREE(e);
            }
        } else if (rootValue == "new") {
            auto line = ast->rValue()->get(0)->line();
            string name(ast->rValue()->get(0)->value());
            ast->rValue()->get(0)->value(".");
            ast->rValue()->get(0)->insert(name, line);
            ast->rValue()->get(0)->insert(".", line);
            ast->rValue()->get(0)->get(1)->insert("prototype", line);
            ast->rValue()->get(0)->get(1)->insert("ctor", line);
            ast->rValue()->value("call");
            Interpreter ip("", filename, this);
            ip.ast->root = ast->rValue();
            ip.child = true;
            auto e = ip.run();
            CHECKITER(e, ast->rValue());
            CHECKPASSNEXTOP(e);
            auto classValue = get(name);
            auto ret = e->get(PASS_RETURN);
            auto keys = classValue->value()->get("prototype")->memberNames();
            for (auto iter = keys.begin(); iter != keys.end(); iter++) {
                auto proto = classValue->value()->get("prototype")->get(*iter)->copy();
                ret->set(*iter, proto);
            }
            exports->set(PASS_RETURN, ret);
            FREE(e);
        }
        else { //为表达式
            auto len = ast->rValue()->length();
            if (rootValue == "call") {
                auto r = ast->rValue();
                auto argsNode = r->get(1);
                vector<Object *> args;
                map<string, Object *> hashArg;
                Interpreter getIp("", filename, this);
                getIp.child = true;
                getIp.ast->root = r->get(0);
                auto e = getIp.run();
                if (e->get(PASS_ERROR)) return exports;
                CHECKITER(e, r);
                CHECKPASSNEXTOP(e);
                auto fun_ = e->get(PASS_RETURN);
                for (UL i = 0; i < argsNode->length(); i++) {
                    auto node = argsNode->get(i);
                    if (node->value() == "=") {
                        auto argName = node->get(0)->value();
                        if (node->get(0)->length() > 0) {
                            std::clog << "ReferenceError: Invalid setting with " << argName << "\n\tat <" << filename
                                      << ":" << upscope << ">:"
                                      << ast->line() << std::endl;
                            if (ast) { delete ast;ast = nullptr; }
                            THROW;
                        }
                        auto valueAst = node->get(1);
                        Interpreter ip("", filename, this);
                        ip.child = true;
                        ip.ast->root = valueAst;
                        auto e = ip.run();
                        Object* argValue;
                        CHECKITER(e, ast);
                        OPERPASSNEXTOP(e, argValue);
                        hashArg.insert(std::pair<string, Object *>(argName, argValue));
                    } else {
                        auto valueAst = node;
                        Interpreter ip("", filename, this);
                        ip.child = true;
                        ip.ast->root = valueAst;
                        auto e = ip.run();
                        CHECKITER(e, ast);
                        Object* argValue;
                        if (fun_->type() == FUNCTION && ((Function*)fun_)->isRefer(node->value())) {
                            argValue = e->get(PASS_RETURN);
                        } else {
                            OPERPASSNEXTOP(e, argValue);
                        }
                        args.push_back(argValue);
                    }
                }
                auto up = e->get(PASS_UPVALUE);
                bool clean = false;
                if (!up) {
                    up = new Undefined;
                    clean = true;
                }
                if (!fun_) {
                    std::clog << "TypeError: The value for getting is not defined\n\tat <" << filename << ":" << upscope << ">:"
                              << ast->line() << std::endl;
                    if (ast) { delete ast;ast = nullptr; }
                    THROW;
                }
                if (fun_->type() == FUNCTION) {
                    auto fun = (Function *) fun_;
                    fun->setParent(this);
                    fun->getParent()->set("this", up);
                    auto fune = fun->run(args, hashArg);
                    exports->set(PASS_RETURN, fune);
                    FREE(fune);
                } else if (fun_->type() == NATIVE_FUNCTION) {
                    auto fun = (NativeFunction *) fun_;
                    fun->setParent(this);
                    fun->getParent()->set("this", up);
                    auto fune = fun->run(args, hashArg);
                    exports->set(PASS_RETURN, fune);
                } else {
                    if (clean) up->unbind();
                    std::clog << "TypeError: The value for getting is not a function\n\tat <" << filename << ":" << upscope << ">:"
                              << ast->line() << std::endl;
                    if (ast) { delete ast;ast = nullptr; }
                    THROW;
                }
//                if (get("this")->value()->type() == OBJECT && fune->type() == UNDEFINED) {
//                    exports->set(PASS_RETURN, get("this")->value());
//                }
                if (clean) up->unbind();
                del("this");
            } else if (rootValue == "get") {
                if (ast->rValue()->length() < 1) {
                    auto name = rootValue;
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
                        Object* keyValue;
                        OPERPASSNEXTOP(e, keyValue);
                        key = keyValue->toString(false, false);
                        up = value;
                        value = value->get(key);
                        if (!value) {
                            if (i == ast->rValue()->length() - 1) {
                                value = new Undefined;
                                up->set(key, value);
                            } else {
                                std::clog << "ReferenceError: Cannot get property "
                                          << e->get(PASS_RETURN)->toString(false, false) << ", it is not defined\n\tat <"
                                          << filename << ":" << upscope << ">:"
                                          << ast->line() << std::endl;
                                if (ast) { delete ast;ast = nullptr; }
                                THROW;
                            }
                        }
                    }
                    exports->set(PASS_RETURN, value);
                    exports->set(PASS_UPVALUE, up);
                    exports->set(PASS_LASTKEY, new String(key));
                }
            } else if (rootValue == ".") {
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
            } else if (rootValue == "o-value") {
                auto object = new Object;
                for (L i = 0; i < ast->rValue()->length(); i++) {
                    string key(ast->rValue()->get(i)->value());
                    auto valueNode = ast->rValue()->get(i)->get(0);
                    string valueStr(valueNode->value());
                    Interpreter ip(valueStr, filename, this);
                    auto e = ip.run();
                    CHECKITER(e, valueNode);
                    CHECKPASSNEXTOP(e);
                    object->set(key, e->get(PASS_RETURN));
                    FREE(e);
                }
                exports->set(PASS_RETURN, object);
            } else if (rootValue == "a-value") {
                auto ret = new Object;
                ret->set("ctor", new Function("ctor", "let this = {};return this;"));
                auto pushFun = new Function("push", "this[this.__len] = n;this.__len++;");
                pushFun->addDesc("n");
                ret->set("push", pushFun);
                L arrlen = 0;
                ret->set("__SYSTEM_TYPE__", new String("Array"));
                for (L i = 0; i < ast->rValue()->length(); i++) {
                    auto valueNode = ast->rValue()->get(i);
                    string valueStr(ast->rValue()->get(i)->value());
                    Interpreter ip(valueStr, filename, this);
                    auto e = ip.run();
                    CHECKITER(e, valueNode);
                    CHECKPASSNEXTOP(e);
                    ret->set(std::to_string(arrlen++), e->get(PASS_RETURN));
                    FREE(e);
                }
                ret->set("__len", new Number(arrlen));
                exports->set(PASS_RETURN, ret);
            } else if (len < 1) {
                if (isNumber(rootValue)) {
                    exports->set(PASS_RETURN, new Number(transSD(rootValue)));
                } else if (isString(rootValue)) {
                    auto s = rootValue;
                    s.erase(0, 1);
                    s.erase(s.length() - 1, 1);
                    exports->set(PASS_RETURN, (new String(s))->trans());
                } else {
                    auto name = rootValue;
                    auto var = scope->get(name);
                    NOTDEFINED(var, name);
                    exports->set(PASS_RETURN, var->value());
                }
            } else if (len == 1) {
                if (rootValue == "++" || rootValue == "--" || rootValue == "++-f" || rootValue == "---f") {
                    Interpreter ip("", filename, this);
                    ip.ast->root = ast->rValue()->get(0);
                    ip.child = true;
                    auto e = ip.run();
                    CHECKITER(e, ast);
                    CHECKPASSNEXTOP(e);
                    auto n = e->get(PASS_RETURN);
//                std::cout << n->type() << std::endl;
                    if (rootValue == "++") {
                        if (n->type() == NUMBER) {
                            Number *v = (Number *) n;
                            exports->set(PASS_RETURN, v->copy());
                            v->value()++;
                        } WRONGEXPRTYPE(rootValue);
                    } else if (rootValue == "--") {
                        if (n->type() == NUMBER) {
                            Number *v = (Number *) n;
                            exports->set(PASS_RETURN, v->copy());
                            v->value()--;
                        } WRONGEXPRTYPE(rootValue);
                    } else if (rootValue == "++-f") {
                        if (n->type() == NUMBER) {
                            Number *v = (Number *) n;
                            v->value()++;
                            exports->set(PASS_RETURN, v->copy());
                        } WRONGEXPRTYPE(rootValue);
                    } else if (rootValue == "---f") {
                        if (n->type() == NUMBER) {
                            Number *v = (Number *) n;
                            v->value()--;
                            exports->set(PASS_RETURN, v->copy());
                        } WRONGEXPRTYPE(rootValue);
                    }
                    FREE(e);
                } else {
                    auto valueAst = ast->rValue()->get(0);
                    auto valueIp = new Interpreter("", filename, this);
                    valueIp->ast->root = valueAst;
                    valueIp->child = true;
                    auto vipe = valueIp->run();
                    auto e_ = vipe->get(PASS_RETURN);
                    delete valueIp;

                    auto op = rootValue;
                    auto etype = e_->type();
                    if (etype == NUMBER) {
                        auto e = (Number *) e_;
                        double &s = e->value();
                        if (op == "!") {
                            s = (bool) s;
                        } else if (op == "~") {
                            s = ~((LL) s);
                        }
                        exports->set(PASS_RETURN, e);
                        delete vipe;
                    } else if (etype == OBJECT || etype == STRING || etype == NATIVE_FUNCTION || etype == FUNCTION || etype == NATIVE_VALUE) {
                        if (op == "!") exports->set(PASS_RETURN, new Number(0));
                        WRONGEXPRTYPE(op);
                    } else if (etype == NULL_ || etype == UNDEFINED) {
                        if (op == "!") exports->set(PASS_RETURN, new Number(1));
                        WRONGEXPRTYPE(op);
                    } WRONGEXPRTYPE(op);
                    FREE(e_);
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
                string op(rootValue);
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
                    if ((leftNode->length() > 0 && name != "." && name != "get") || isNumber(name) || isString(name) || name == "o-value" || name == "a-value") {
                        std::clog << "ReferenceError: Invalid left-hand side in assignment\n\tat <" << filename << ":" << upscope << ">:"
                                  << ast->line() << std::endl;
                        if (ast) { delete ast;ast = nullptr; }
                        THROW;
                    }
                    Object* up;
                    string key;
                    if (name == "." || (name == "get" && leftNode->length() > 0)) {
                        Interpreter ip("", filename, this);
                        ip.child = true;
                        ip.ast->root = leftNode;
                        auto e = ip.run();
                        CHECKITER(e, leftNode);
                        CHECKPASSNEXTOP(e);
                        up = e->get(PASS_UPVALUE);
                        key = ((String*)e->get(PASS_LASTKEY))->value();
                        if (op == "=") up->set(key, right);
                        exports->set(PASS_RETURN, right);
                    }
                    if (op == "=" && !(name == "." || (name == "get" && leftNode->length() > 0))) {
                        auto snV = scope->get(name)->value();
                        auto snType = snV->type();
                        if (snType == right->type()) {
                            switch (snType) {
                                case STRING:
                                {
                                    String* snVS = (String*)snV;
                                    String* rightS = (String*)right;
                                    snVS->value() = rightS->value();
                                    break;
                                }
                                case NUMBER:
                                {
                                    Number* snVS = (Number*)snV;
                                    Number* rightS = (Number*)right;
                                    snVS->value() = rightS->value();
                                    break;
                                }
                                default:
                                {
                                    scope->set(name, right);
                                    break;
                                }
                            }
                        } else scope->set(name, right);
                        exports->set(PASS_RETURN, right);
                    } else {
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
                        exports->set(PASS_RETURN, value_);
                    }
                    FREE(left);
                    FREE(right);
                } else if (
                        op == "==" || op == "<=" || op == ">=" || op == "<" || op == ">" || op == "!="
                        ) {
                    if (left->type() != right->type()) {
#define CHECK_EQ_DIF_TYPE_MACRO(t) ((left->type() == t && right->type() == NULL_) || \
                        (left->type() == NULL_ && right->type() == t) || \
                        (left->type() == UNDEFINED && right->type() == t) || \
                        (left->type() == t && right->type() == UNDEFINED))
                        if (
                                CHECK_EQ_DIF_TYPE_MACRO(OBJECT)
                                || CHECK_EQ_DIF_TYPE_MACRO(NATIVE_VALUE)
                                || CHECK_EQ_DIF_TYPE_MACRO(FUNCTION)
                                || CHECK_EQ_DIF_TYPE_MACRO(NATIVE_FUNCTION)
                                ) {
                            if (op == "!=") exports->set(PASS_RETURN, new Number(1));
                            else exports->set(PASS_RETURN, new Number(0));
                        } else {
                            std::clog << "TypeError" << ": " << "Cannot compare values with two different types" << "\n\tat <" << filename << ":" << upscope << ">:" << ast->line() << std::endl;
                            if (ast) { delete ast;ast = nullptr; }
                            THROW;
                        }
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
                        else if (op == "~~") {
                            auto arr = new Object();
                            arr->set("ctor", new Function("ctor", "let this = {};return this;"));
                            auto pushFun = new Function("push", "this[this.__len] = n;this.__len++;");
                            pushFun->addDesc("n");
                            arr->set("push", pushFun);
                            arr->set("__SYSTEM_TYPE__", new String("Array"));
                            leftV = (ULL)leftV;
                            rightV = (ULL)rightV;
                            arr->set("__len", new Number(rightV - leftV));
                            for (ULL v = leftV, i = 0; v < rightV; v++, i++) {
                                arr->set(std::to_string(i), new Number(v));
                            }
                            exports->set(PASS_RETURN, arr);
                        }
                    } WRONGEXPRTYPE(op);
                }
            }
        }
        if (child) break;
    }
    if (!exports->get(PASS_RETURN)) exports->set(PASS_RETURN, new Undefined);
    if (!child && ast) {
        { delete ast;ast = nullptr; }
    }
    return exports;
}

BM::Object *BM::Interpreter::runCC() {
    ast->importByString(script);
    return run();
}

void BM::Interpreter::import(Object* exports, const string& name, const string& asName) {
    if (scope->get(asName)) {
        std::clog << "ImportError: '" << asName << "' is in usage \n\tat <" << filename << ">:" << ast->line() << std::endl;
        exports->set(PASS_ERROR, new Number(1));
        return;
    }
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
            ip.set(PASS_MODULE_NAME, new String(name));
            auto moduleExports = ip.run();
            if (moduleExports->get(PASS_ERROR)) {
                std::clog << "ImportError: Module script wrong\n\tat <" << filename << ">:" << ast->line() << std::endl;
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
            ip.set(PASS_MODULE_NAME, new String(name));
            auto moduleExports = ip.run();
            if (moduleExports->get(PASS_ERROR)) {
                std::clog << "ImportError: Module script wrong\n\tat <" << filename << ">:" << ast->line() << std::endl;
                exports->set(PASS_ERROR, new Number(1));
                return;
            }
            moduleExports->del(PASS_RETURN);
            scope->set(asName, moduleExports);
        }
    }
    // 为全局模块
    if (!finish) {
        string path(BMLMPATH + name + (name[nameLen - 1] == '/' ? "init.bm" : "/init.bm"));
        file.open(path);
        if (file) {
            finish = true;
            while (getline(file, tmpLine)) {
                script += tmpLine + "\n";
            }
            Interpreter ip(script, name);// import的文件是独立运行的，不与import它的脚本连接
            auto moduleName = new String(name);
            ip.set(PASS_MODULE_NAME, moduleName);
            ip.set("null", new Null);
            ip.set("undefined", new Undefined);
            auto moduleExports = ip.run();
            if (moduleExports->get(PASS_ERROR)) {
                std::clog << "ImportError: Module script wrong\n\tat <" << filename << ">:" << ast->line() << std::endl;
                exports->set(PASS_ERROR, new Number(1));
                return;
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
        } else std::clog << "ImportError" << ": No module named " << name << "\n\tat <" << filename <<  ":" << upscope << ">:" << ast->line() << std::endl;
        dylib.close();
    }
}
void BM::Interpreter::Using(Object* exports, AST::node* objAst) {
    Interpreter ip("", filename, this);
    ip.ast->root = objAst;
    ip.child = true;
    auto e = ip.run();
    if (!e || e->get(PASS_ERROR)) std::clog << "at <" << filename << ":" << upscope << ">:" << ast->line() << std::endl;
    auto obj = e->get(PASS_RETURN);
    Object::Iterator iter(obj);
    for (; !iter.end(); iter.next()) {
        set(iter.key(), iter.value());
    }
}