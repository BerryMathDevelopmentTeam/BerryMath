#include <iostream>
#include <ctime>
#include <BerryMath.h>
#include <dlfcn.h>
//#include <benchmark/benchmark.h>

using dllFun = int(*)(int, int);

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
    BM::AST ast("a(\"123\", 3, \"hello\" * (23 + 4));a += 1;");
    ast.parse();

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
void TEST6() {
    BM::AST ast("while (a > 0) { println(\"Hello!\");a++; }");
    ast.parse();

}
void TEST7() {
    BM::AST ast("do {println(a);a++;} while (a > 10);");
    ast.parse();

}
void TEST8() {
    BM::AST ast("for (let i = 0; i < 15; i += 1) { println(i); }");
    ast.parse();

}
void TEST9() {
    BM::AST ast("def a(b = 34 * (1 + 4), public c = b(123), private d = 3, private e) {\nprintln(b, c, d, e);\n}");
    ast.parse();

}
void TEST10() {
    BM::AST ast("(++a)");
    ast.parse();

}
void TEST11() {
    BM::Dylib dylib("libadd");
    dllFun add = (dllFun)dylib.resolve("add");
    std::cout << add(1, 2) << std::endl;
}
void TEST12() {
    BM::AST ast("continue;");
    ast.parse();

}
void TEST13() {
    BM::AST ast("a[\"123\" + \"Hello\"][123 + 123 * 2]");
    ast.parse();

}
void TEST14() {
    BM::AST ast("import \"Hello-\" + name as Hello;");
    ast.parse();

}
void TEST15() {
    BM::AST ast("(-a);");
    ast.parse();

}
void TEST16() {
    BM::Scope scope;
    scope.set("bat", new BM::Number(123));
    std::cout << scope.get("bar")->value()->toString() << std::endl;

}
void TEST17() {
    BM::AST ast("delete a;");
    ast.parse();

}
void TEST18() {
    BM::AST ast("if (a == 0) {\nprintln(\"eq 0\");\n} elif (a == 2) {\nprintln(\"eq 2\");\n} elif (a == 3) {\nprintln(\"eq 3\");\n} else {\nprintln(\"none\");\n}");
    ast.parse();
    ast.Export("test.bmast");
}
void TEST19() {
    BM::AST ast;
    ast.import("test.bmast");
    ast.parse();

}
void TEST20() {
    BM::Interpreter interpreter("import \"sys\" as sys;let a = 2 * (7 + 1);a++;sys.print(\"Hello world\", 123, a, a * 4)", "main.bm");
    auto e = interpreter.run();
    e->get("__RETURN__");
    delete e;
}

int main() {
//    TEST1();
//    TEST2();
//    TEST3();
//    TEST4();
//    TEST5();
//    TEST6();
//    TEST7();
//    TEST8();
//    TEST9();
//    TEST10();
//    TEST11();
//    TEST12();
//    TEST13();
//    TEST14();
//    TEST15();
//    TEST16();
//    TEST17();
//    TEST18();
//    TEST19();
    TEST20();

    // Get speed
    /*auto start = clock();
    for (int i = 0; i < 10000; i++) TEST20();
    std::cout << "used " << (clock() - start) / (double)CLOCKS_PER_SEC * (double)1000 << "ms." << std::endl;*/
    return 0;
}