#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <dlfcn.h>
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
//        if (!ast->rValue()) continue;
        if (ast->value() == "PROGRAM-END") break;
        if (ast->value() == "bad-tree") {
            std::cerr << ast->rValue()->get(0)->value() << "at <" << filename << ">:" << ast->line() << std::endl;
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
                            THROW;
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
                            THROW;
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
                        Interpreter ip(script, name);// import的文件是独立运行的，与import它的脚本连接
                        auto moduleExports = ip.run();
                        if (moduleExports->get(PASS_ERROR)) {
                            std::cerr << "ImportError: Module script wrong at <" << filename << ">:" << ast->line() << std::endl;
                            THROW;
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
                    } WRONG("ImportError", "No module named " + name);
                    dylib.close();
                }
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
                            delete e;
                            break;
                        }
                    } else {
                        string script(elconAst->get(0)->value());
                        Interpreter scriptIp(script, filename, this);
                        auto e = scriptIp.run();
                        CHECKITER(e, scriptIp.ast);
                        delete e;
                        break;
                    }
                }
            }
            delete conE;
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
                                      << ">:"
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
                if (!fun_) {
                    std::cerr << "TypeError: The value for getting is not defined at <" << filename << ">:"
                              << ast->line() << std::endl;
                    THROW;
                }
                if (fun_->type() == FUNCTION) {
                    auto fun = (Function *) fun_;
                    exports->set(PASS_RETURN, fun->run(args, hashArg));
                } else if (fun_->type() == NATIVE_FUNCTION) {
                    auto fun = (NativeFunction *) fun_;
                    exports->set(PASS_RETURN, fun->run(args, hashArg));
                } else {
                    std::cerr << "TypeError: The value for getting is not a function at <" << filename << ">:"
                              << ast->line() << std::endl;
                    THROW;
                }
            } else if (ast->value() == "get") {
                string name(ast->rValue()->get(0)->value());
                auto v = scope->get(name);
                NOTDEFINED(v, name);
                BM::Object* value = v->value();
                for (UL i = 1; i < ast->rValue()->length(); i++) {// 下标为0的是v的名字
                    Interpreter ip("", filename, this);
                    ip.ast->root = ast->rValue()->get(i);
                    ip.child = true;
                    auto e = ip.run();
                    CHECKITER(e, ast);
                    value = value->get(e->get(PASS_RETURN)->toString(false, false));
                    if (!value) {
                        std::cerr << "ReferenceError: Cannot get property " << e->get(PASS_RETURN)->toString(false, false) << " is not defined at <" << filename << ">:"
                                  << ast->line() << std::endl;
                        THROW;
                    }
                }
                exports->set(PASS_RETURN, value);
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
                    auto value = var->value();
                    for (UL i = 0; i < keys.size(); i++) {
                        value = value->get(keys[i]);
                        if (value) { } WRONG("ReferenceError", keys[i] + " is not defined in " + startV + " or its properties");
                    }
                    exports->set(PASS_RETURN, value);
                } WRONG("ReferenceError", startV + " is not defined");
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
                    NOTDEFINED(var, name);
                    exports->set(name, var->value());
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
                        exports->set(PASS_RETURN, new String(leftV + rightV));
                    } WRONGEXPRTYPE(op);
                } else if (
                        op == "=" || op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "**=" ||
                        op == "<<=" || op == ">>=" || op == "|=" || op == "&=" || op == "^="
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
                                    Number *value = (Number *) value_;
                                    value->value() += ((Number *) right)->value();
                                } WRONGEXPRTYPE(op);
                            } else if (value_->type() == STRING) {
                                if (right->type() == STRING) {
                                    String *value = (String *) value_;
                                    value->value() += ((String *) right)->value();
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
                        std::cerr << "TypeError" << ": " << "Cannot compare values with two different types" << " at <" << filename << ">:" << ast->line() << std::endl;
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
    scope->clear();
    if (!exports->get(PASS_RETURN)) exports->set(string(PASS_RETURN), new Undefined);
    return exports;
}

BM::Object *BM::Interpreter::runCC() {
    ast->importByString(script);
    return run();
}