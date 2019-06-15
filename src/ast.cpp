#include <iostream>
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
        case Lexer::STRING_TOKEN:
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
        case Lexer::BRACKETS_LEFT_TOKEN:
        {
            UL leftLine;
            string left;
            string op;
            string right;
            leftLine = lexer.line();
            auto bcount = 1;
            GET;
            while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                    bcount++;
                }
                if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    bcount--;
                    if (bcount < 1) break;
                }
                left += token.s;
                GET;
            }
            if (token.t == Lexer::END_TOKEN || token.t == Lexer::PROGRAM_END) {
                auto ast = new AST(left, leftLine + baseLine);
                ast->parse();
                root = ast->root;
                delete ast;
                break;
            }
            GET;
            if (!(token.t > Lexer::NOTE_TOKEN && token.t < Lexer::END_TOKEN)) {
                root->value("err");
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.line() + baseLine);
                break;
            }
            auto opLine = lexer.line();
            op = token.s;
            auto rightLine = lexer.line();
            do {
                GET;
                right += token.s;
            } while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END);
            auto leftAST = new AST(left, leftLine + baseLine);
            auto rightAST = new AST(right, rightLine + baseLine);
            leftAST->parse();
            rightAST->parse();
            root = new node(op, opLine + baseLine);
            root->insert(leftAST->root);
            root->insert(rightAST->root);
            delete leftAST;
            delete rightAST;
            break;
        }
    }
}

UL BM::AST::priority(const string& op) {
    if (op == "(" || op == "[" || op == ".") return 14;
    if (op == "++" || op == "--" || op == "!" || op == "~") return 13;
    if (op == "*" || op == "/" || op == "%") return 12;
    if (op == "+" || op == "-") return 11;
    if (op == "<<" || op == ">>") return 10;
    if (op == ">" || op == ">=" || op == "<" || op == "<=") return 9;
    if (op == "==" || op == "!=") return 8;
    if (op == "&") return 7;
    if (op == "^") return 6;
    if (op == "|") return 5;
    if (op == "&&") return 4;
    if (op == "||") return 3;
    if (
            op == "="
            || op == "*=" || op == "/=" || op == "%="
            || op == "+=" || op == "-="
            || op == ">>=" || op == "<<="
            || op == "^=" || op == "&=" || op == "|="
            )
        return 2;
    if (op == ",") return 1;
    return 15;
}