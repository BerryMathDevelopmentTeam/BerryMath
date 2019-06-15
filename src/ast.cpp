#include "ast.h"

void BM::AST::parse() {
    Lexer lexer(script);
    auto token = lexer.get();
#define GET token = lexer.get()
    switch (token.t) {
        case Lexer::LET_TOKEN:
        {
            root = new node("let", lexer.line() + baseLine - 1);// 因为行数起步都是1, 所以如果不减1, 就会导致最后行数错位, 多1
            GET;
            if (token.t != Lexer::UNKNOWN_TOKEN) {
                root->value("err");
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.line() + baseLine);
                return;
            }
            root->insert(token.s, lexer.line());
            GET;
            if (token.t != Lexer::SET_TOKEN) {
                root->value("err");
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.line() + baseLine);
                return;
            }
            string expression("");
            do {
                GET;
                expression += token.s;
            } while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END);
            auto ast = new AST(expression, lexer.line() + baseLine);
            ast->parse();
            root->insert(ast->root);
            delete ast;
            break;
        }
        case Lexer::NUMBER_TOKEN:
        {
            auto tmpIndex = lexer.index();
            auto tmpLine = lexer.line();
            auto tmp = token;
            GET;
            if (token.t > Lexer::NOTE_TOKEN && token.t < Lexer::END_TOKEN) {
                auto op = token;
                auto opLine = lexer.line();
                string expression("");
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    expression += token.s;
                    GET;
                }
                auto ast = new AST(expression, lexer.line() + baseLine);
                ast->parse();
                root = new node(op.s, opLine + baseLine);
                root->insert(tmp.s, tmpLine + baseLine);
                root->insert(expression, lexer.line() + baseLine);
                delete ast;
            } else {
                lexer.i = tmpIndex;
                lexer.l = tmpLine;
                root = new node(tmp.s, lexer.line() + baseLine - 1);
            }
            break;
        }
    }
}