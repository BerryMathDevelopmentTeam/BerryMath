#include <iostream>
#include <sstream>
#include <BerryMath.h>
#include <vector>
#include <map>
#include "library.h"
using std::vector;
using std::map;
using BM::Object;
using BM::Scope;

Object* print(BM::Scope* scope, vector<Object*> unknowns) {
    std::ios_base::sync_with_stdio(false);
    auto sp = scope->get("sp")->value()->toString(true, false);
    if (!unknowns.empty()) {
        UL i = 0;
        for (; i < unknowns.size() - 1; i++) {
            std::cout << unknowns[i]->toString(true, false) << sp;
            std::ios_base::sync_with_stdio(false);
        }
        std::cout << unknowns[i]->toString(true, false) << scope->get("end")->value()->toString(true, false);
    }
    std::ios_base::sync_with_stdio(false);
    return new BM::Undefined;
}
Object* input(BM::Scope* scope, vector<Object*> unknowns) {
    auto sp = scope->get("sp")->value()->toString(true, false);
    if (!unknowns.empty()) {
        UL i = 0;
        for (; i < unknowns.size() - 1; i++) {
            std::cout << unknowns[i]->toString(true, false) << sp;
        }
        std::cout << unknowns[i]->toString(true, false);
    }
    string t;
    std::cin >> t;
    return new BM::String(t);
}
Object* number(BM::Scope* scope, vector<Object*> unknowns) {
    auto value = scope->get("value")->value();
    if (value->type() == BM::NUMBER) return value->copy();
    if (value->type() == BM::NULL_ || value->type() == BM::UNDEFINED || value->type() == BM::NATIVE_FUNCTION || value->type() == BM::FUNCTION || value->type() == BM::OBJECT) return new BM::Number(1);
    std::stringstream ss;
    auto v = (BM::String*) value;
    ss << v->value();
    double t;
    ss >> t;
    return new BM::Number(t);
}
Object* range(BM::Scope* scope, vector<Object*> unknowns) {
    auto s_ = scope->get("s")->value();
    auto e_ = scope->get("e")->value();
    auto step_ = scope->get("step")->value();
    BM::Interpreter ip("[]");
    auto ex = ip.run();
    auto ret = ex->get("__RETURN__");
    ret->bind();
    if (s_->type() != BM::NUMBER || e_->type() != BM::NUMBER || step_->type() != BM::NUMBER) return ret;
    auto s = ((BM::Number*)s_)->value();
    auto e = ((BM::Number*)e_)->value();
    auto step = ((BM::Number*)step_)->value();
    double i = s;
    unsigned long long v = ((BM::Number*)ret->get("__len"))->value();
    while (i < e) {
        ret->set(std::to_string(v), new BM::Number(i));
        v++;
        i += step;
    }
    ((BM::Number*)ret->get("__len"))->value() = v;
    delete ex;
    return ret;
}
Object* String(BM::Scope* scope, vector<Object*> unknowns) {
    return new BM::String(scope->get("value")->value()->toString(false, false));
}

BM::Object* initModule() {
    // 创建exports
    auto exports = new BM::Object;

    // 设置模块分区
    exports->set("io", new BM::Object);
    exports->set("type", new BM::Object);
    exports->set("platform", new BM::Object);
    exports->set("BM", new BM::Object);

    // 为io分区添加函数
    auto printP = new BM::NativeFunction("print", print);// 创建拓展函数
    auto inputP = new BM::NativeFunction("input", input);// 创建拓展函数
    // 为printP设置默认值
    printP->defaultValue("end", new BM::String("\n"));
    printP->defaultValue("sp", new BM::String(" "));
    // 为inputP设置默认值
    inputP->defaultValue("sp", new BM::String(" "));
    exports->get("io")->set("print", printP);
    exports->get("io")->set("input", inputP);

    // 为type分区添加函数
    auto numberP = new BM::NativeFunction("number", number);// 创建拓展函数
    auto stringP = new BM::NativeFunction("string", String);// 创建拓展函数
    numberP->addDesc("value");
    stringP->addDesc("value");
    exports->get("type")->set("number", numberP);
    exports->get("type")->set("string", stringP);

    // 为type分区添加函数
    auto rangeP = new BM::NativeFunction("range", range);// 创建拓展函数
    rangeP->addDesc("s");
    rangeP->addDesc("e");
    rangeP->addDesc("step");
    rangeP->defaultValue("step", new BM::Number(1));
    exports->set("range", rangeP);

#if defined(I_OS_DARWIN)
    exports->get("platform")->set("osName", new BM::String("darwin"));
#if defined(I_OS_DARWIN64)
    exports->get("platform")->set("bit", new BM::Number(64));
#else
    exports->get("platform")->set("bit", new BM::Number(32));
#endif
#elif defined(I_OS_WIN)
    exports->get("platform")->set("osName", new BM::String("windows"));
#if defined(I_OS_WIN64)
    exports->get("platform")->set("bit", new BM::Number(64));
#else
    exports->get("platform")->set("bit", new BM::Number(32));
#endif
#elif defined(I_OS_LINUX)
    exports->get("platform")->set("osName", new BM::String("linux"));
    exports->get("platform")->set("bit", new BM::Number(32));
#elif defined(I_OS_UNIX)
    exports->get("platform")->set("osName", new BM::String("unix"));
    exports->get("platform")->set("bit", new BM::Number(32));
#endif

    exports->get("BM")->set("version", new BM::String(BMVersion));
    exports->get("BM")->set("author", new BM::String("BerryMathDevelopmentTeam"));
    return exports;// 导出模块
}