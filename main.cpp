#include <iostream>
#include <BerryMath.h>

void TEST1() {
    auto object = new BM::Object;
    auto child = new BM::Object;
    object->insert("number", new BM::Number(0));
    object->insert("string", new BM::String("Hello world!"));
    object->insert("p", new BM::Null);
    object->insert("v", new BM::Undefined);
    object->insert("child", child);
    child->insert("Text", new BM::String("I keep my ideas"));
    std::cout << (*object) << std::endl;
    delete object;
}
void TEST2() {
    string v("let a = 1.0;");
    BM::Lexer lexer(v);
    auto t = lexer.get();
    std::cout << t.s << std::endl;
    while (t.t != BM::Lexer::END_TOKEN) {
        t = lexer.get();
        std::cout << t.s << std::endl;
    }
}

int main() {
//    TEST1();
    TEST2();
    return 0;
}