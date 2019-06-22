#include <iostream>
#include <ctime>
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
    string v("let\na1 = 0;a1++;");
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
    BM::AST ast("if (a == 0) {\nprintln(\"eq 0\");\n} elif (a == 2) {\nprintln(\"eq 2\");\n} elif (a == 3) {\nprintln(\"eq 3\");\n} else {\nprintln(\"none\");\n}");
    ast.parse();

}
void TEST5() {
    BM::AST ast(
            "switch (a) {\n"
            "case 1:\n"
            "case 2:\n"
            "\tprintln(\"#1\");\n"
            "\tbreak;\n"
            "default:\n"
            "\tprintln(\"#2\");\n"
            "}"
    );
    ast.parse();

}


int main() {
//    TEST1();
//    TEST2();
//    TEST3();
//    TEST4();
    TEST5();

    // Get speed
    /*auto start = clock();
    for (int i = 0; i < 10000; i++) TEST4();
    std::cout << "used " << (clock() - start) / (double)CLOCKS_PER_SEC * (double)1000 << "ms." << std::endl;*/
    return 0;
}