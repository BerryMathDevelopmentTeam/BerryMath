#include <iostream>
#include <regex>
#include "color.h"
#include "memory.h"
#include "AST.h"
#include "script.h"
using std::to_string;

BerryMath::value* BerryMath::script::run(long line) {
    int i = 0;
    scope = new block();
    auto ret = new value();
    string c("");
//    std::regex replacer_self_add("++");
//    code = std::regex_replace(code, replacer_self_add, "+= 1");
    int bigBracketsCount = 0;
    bool noBrackets = true;
//    std::cout << code << std::endl;
    while (true) {
        if (i >= code.length()) break;
        c += code[i];
//        std::cout << code[i];
        if (code[i] == '\n') {
            line++;
        }
        if (code[i] == '{') {
            noBrackets = false;
            bigBracketsCount++;
        }
        if (code[i] == '}') {
            noBrackets = false;
            bigBracketsCount--;
        }
        if (code[i] == ';' && noBrackets) {// 单行语句
//            std::cout << "build AST." << std::endl;
            ast = new BerryMath::AST(c);
            ast->parse();
            auto root = ast->value();
            parse(ret, root, line);
        }
        if (!noBrackets && bigBracketsCount == 0) {// 包含大括号的诸如if语句等
            auto ast = new BerryMath::AST(c);
            ast->parse();
            c = "";
        }
        i++;
    }
    if (selfAst) {
        auto root = ast->value();
        parse(ret, root, line);
    }
    if (code.length() >= 11) {
        std::cout << "[Program finish]" << std::endl;
        std::cout << "=========Hash Table=========" << std::endl;
        scope->each([](variable* var) -> void {
            std::cout << var->nameOf() << ": " << var->valueOf().valueOf() << std::endl;
        });
        std::cout << "=======Hash Table End=======" << std::endl;
    }
    return ret;
}

void BerryMath::script::parse(value*& ret, AST::ASTNode *root, long line) {
    auto now = root;
    if (root->value() == "bad-tree") {
        // 错误的树
        Throw(line, "SyntaxError: Bad tree");
    }
    if (root->value() == "root") {
        now = root->at(-1);
    }
    string op = now->value();
    if (now->children.size() == 0) {
        delete ret;
        ret = new value(op);
    }
    if (op == "=") {
        string name = now->at(0)->value();
        auto s = new script(new AST(now->at(1)));
        auto v = s->run(line);
        scope->set(name, v);
//                std::cout << "set: " << name << std::endl;
    }
    if (op == "~") {
        auto s = new script(new AST(now->at(0)));
        auto r = s->run(line);
        if (r->typeOf() != NUMBER) {
            Throw(line, "SyntaxError: Wrong expression");
            return;
        }
        int n = (int) atof(r->valueOf().c_str());
        delete ret;
        ret = new value(to_string(~n));
//                std::cout << "set: " << name << std::endl;
    }
    if (op == "!") {
        auto s = new script(new AST(now->at(0)));
        auto r = s->run(line);
        bool n = isTrue(r->valueOf());
        delete ret;
        if (n) {
            ret = new value("0");
        } else {
            ret = new value("1");
        }
//                std::cout << "set: " << name << std::endl;
    }
    if (
            op == "*" || op == "/" || op == "%"
            || op == "+" || op == "-"
            || op == ">>" || op == "<<"
            || op == "&" || op == "|" || op == "^"
            || op == ">" || op == ">="
            || op == "<" || op == "<="
            ) {// 四则运算
        auto ls = new script(new AST(now->at(0)));
        auto rs = new script(new AST(now->at(1)));
        auto l = ls->run(line);
        auto r = rs->run(line);
        if (l->typeOf() == STRING && r->typeOf() == STRING && op == "+") {// 字符串加法
            string lv = l->valueOf();
            string rv = r->valueOf();
            // 去除引号
            lv.erase(lv.begin());
            lv.erase(lv.end() - 1);
            rv.erase(rv.begin());
            rv.erase(rv.end() - 1);
            string res = lv + rv;
            delete ret;
            ret = new value(STRING, "\"" + res + "\"");
        } else {
            if (l->typeOf() != NUMBER || r->typeOf() != NUMBER) {
                Throw(line, "TypeError: Unable to run expression because this expression must be (number operator number)");
                return;
            }
            double lv = atof(l->valueOf().c_str());
            double rv = atof(r->valueOf().c_str());
            double res = 0;
            if (op == "*") res = lv * rv;
            if (op == "/") res = lv / rv;
            if (op == "%") res = ((int)lv) % ((int)rv);
            if (op == "+") res = lv + rv;
            if (op == "-") res = lv - rv;
            if (op == "<<") res = ((int)lv) << ((int)rv);
            if (op == ">>") res = ((int)lv) >> ((int)rv);
            if (op == "&") res = ((int)lv) & ((int)rv);
            if (op == "|") res = ((int)lv) | ((int)rv);
            if (op == "^") res = ((int)lv) ^ ((int)rv);
            if (op == "<") res = lv < rv;
            if (op == "<=") res = lv <= rv;
            if (op == ">") res = lv > rv;
            if (op == ">=") res = lv >= rv;
            delete ret;
            ret = new value(NUMBER, to_string(res));
        }
    }
    if (op == "&&" || op == "||") {
        auto ls = new script(new AST(now->at(0)));
        auto rs = new script(new AST(now->at(1)));
        auto l = ls->run(line);
        auto r = rs->run(line);
        bool lv = isTrue(l->valueOf());
        bool rv = isTrue(r->valueOf());
        int res;
        if (op == "&&") {
            res = lv && rv;
        }
        if (op == "||") {
            res = lv || rv;
        }
        delete ret;
        ret = new value(to_string(res));
    }
    if (
            op == "*=" || op == "/=" || op == "%="
            || op == "+=" || op == "-="
            || op == "&=" || op == "|=" || op == "^="
            || op == "<<=" || op == ">>="
            ) {
        string name = now->at(0)->value();
        auto s = new script(new AST(now->at(1)));
        auto v = s->run(line);
        auto nameValue = scope->of(name);
        if (nameValue == nullptr) {
            Throw(line, "NameError: name '" + name + "' not defined");
        }
    }
}
void BerryMath::script::Throw(long line, string message) {
    std::cout << RED << message << " at line " << (line + 1) << ". " << RESET << std::endl;
}
