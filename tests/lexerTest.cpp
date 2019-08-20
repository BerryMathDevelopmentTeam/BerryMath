#include <BerryMath.h>
#include <gtest/gtest.h>

TEST(BerryMathLexer, open) {
    BM::Lexer lexer;
    lexer.open("a=a++;def fun(a=9,b=10){if(a==0)return b;return fun(a-1, b+1)+a*b;");
    string s;
    string ts;
    while (true) {
        auto tk = lexer.get();
        if (tk.t == BM::Lexer::PASS_TOKEN) break;
        s += tk.s + " ";
        ts += std::to_string(tk.t);
    }
    EXPECT_EQ("a = a ++ ; def fun ( a = 9 , b = 10 ) { if ( a == 0 ) return b ; return fun ( a - 1 , b + 1 ) + a * b ; ", s);
    EXPECT_EQ("1331757931411332948133294245441135294215179151411572948156294256158179", ts);
}
TEST(BerryMathLexer, index) {
    BM::Lexer lexer("let a=0;");
    while (true) {
        if (lexer.get().t == BM::Lexer::UNKNOWN_TOKEN) break;
    }
    EXPECT_EQ(5, lexer.index());
}
TEST(BerryMathLexer, line) {
    BM::Lexer lexer("let\na=\n0\n;");
    while (true) {
        if (lexer.get().t == BM::Lexer::NUMBER_TOKEN) break;
    }
    EXPECT_EQ(2, lexer.line());
}
TEST(BerryMathLexer, get0) {
    BM::Lexer lexer("(((4+3>9*10)*7)**2)>>10");
    string s;
    string ts;
    while (true) {
        auto tk = lexer.get();
        if (tk.t == BM::Lexer::PASS_TOKEN) break;
        s += tk.s + " ";
        ts += std::to_string(tk.t);
    }
    EXPECT_EQ("( ( ( 4 + 3 > 9 * 10 ) * 7 ) ** 2 ) >> 10 ", s);
    EXPECT_EQ("41414129562940295829425829426129426329", ts);
}
TEST(BerryMathLexer, get1) {
    BM::Lexer lexer("a=a++;def fun(a=9,b=10){if(a==0)return b;return fun(a-1, b+1)+a*b;");
    string s;
    string ts;
    while (true) {
        auto tk = lexer.get();
        if (tk.t == BM::Lexer::PASS_TOKEN) break;
        s += tk.s + " ";
        ts += std::to_string(tk.t);
    }
    EXPECT_EQ("a = a ++ ; def fun ( a = 9 , b = 10 ) { if ( a == 0 ) return b ; return fun ( a - 1 , b + 1 ) + a * b ; ", s);
    EXPECT_EQ("1331757931411332948133294245441135294215179151411572948156294256158179", ts);
}