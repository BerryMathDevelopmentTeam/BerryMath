#include <iostream>
#include <BerryMath.h>
//#include <benchmark/benchmark.h>


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
    std::cout << (*object)["p"] << std::endl;
    delete object;
}
void TEST2() {
    string v("let\na = 0;a++;");
    BM::Lexer lexer(v);
    auto t = lexer.get();
    std::cout << t.s << std::endl;
    while (t.t != BM::Lexer::PROGRAM_END) {
        t = lexer.get();
        std::cout << t.s << std::endl;
    }
}
void TEST3() {
    BM::AST ast("a(\"123\", \"hello\" * (23 + 4))");
    ast.parse();

}
void TEST4() {
    BM::AST ast("if (a == 0) {\nprintln(\"eq 0\")\n} elif (a == 2) {\nprintln(\"eq 2\")\n} elif (a == 3) {\nprintln(\"eq 3\")\n} else {\nprintln(\"none\")\n}");
    ast.parse();

}

int main() {
//    TEST1();
//    TEST2();
//    TEST3();
    TEST4();
    return 0;
}