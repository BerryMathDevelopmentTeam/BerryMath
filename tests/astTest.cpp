#include <BerryMath.h>
#include <gtest/gtest.h>

// 判断两个ast是否相等的最好方法就是判断两个ast的script是否相同
TEST(BerryMathAST, ctor) {
    BM::AST ast1;
    BM::AST ast2("");
    EXPECT_EQ(ast1.script, ast2.script);
}
TEST(BerryMathAST, open) {
    BM::AST ast1;
    BM::AST ast2("let a = 0;");
    ast1.open("let a = 0;");
    EXPECT_EQ(ast1.script, ast2.script);
}
TEST(BerryMathAST, parse1) {
    BM::AST ast("let a\n=\n0;");
    ast.parse();
    EXPECT_EQ(ast.exportByString(), "node \"let\" line 0 children node \"a\" line 0 children  end node \"0\" line 2 children  end  end ");
}
TEST(BerryMathAST, parse2) {
    BM::AST ast("(a++)+(++a)");
    ast.parse();
    EXPECT_EQ(ast.exportByString(), "node \"let\" line 0 children node \"a\" line 0 children  end node \"0\" line 2 children  end  end ");
}
TEST(BerryMathAST, export) {
    BM::AST ast1;
    BM::AST ast2("let a = 0;");
    ast1.open("let a = 0;");
    ast1.parse();
    ast2.parse();
    EXPECT_EQ(ast1.exportByString(), ast2.exportByString());
}