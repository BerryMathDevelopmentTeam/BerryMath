#include <iostream>
#include "ast.h"

void BM::AST::parse() {
    Lexer lexer(script);
    auto tmpIndex = lexer.i;
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
        case Lexer::BRACKETS_LEFT_TOKEN:
        {
            UL base = 1;
            struct Operator {
                UL pri;
                UL index;
                UL line;
                string op;
            };
            Operator minOp = {15, 0, 0, ""};
            const auto BRACKETS_PRI = priority("(");
            auto opIndex = lexer.i;
            auto leftLine = lexer.line(), rightLine = lexer.line();
            if (token.t == Lexer::BRACKETS_LEFT_TOKEN) base = BRACKETS_PRI;
            auto tmpToken = token;
            auto tmpTokenLine = lexer.line();
            GET;
            while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                if (token.t > Lexer::NOTE_TOKEN && token.t < Lexer::END_TOKEN) {// 是符号
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) base *= BRACKETS_PRI;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) base /= BRACKETS_PRI;
                    else {
                        auto pri = priority(token.s) * base;
                        if (pri < minOp.pri) {
                            minOp.pri = pri;
                            minOp.op = token.s;
                            minOp.index = opIndex;
                            minOp.line = lexer.line();
                            rightLine = lexer.line();
                        }
                    }
                }
                opIndex = lexer.i;
                GET;
            }
            if (minOp.pri == 15 && minOp.index == 0 && minOp.op.empty()) {
                root = new node(tmpToken.s, tmpTokenLine + baseLine);
                return;
            }
            string left, right;
            lexer.i = tmpIndex;
            GET;
            while (lexer.i != minOp.index) {
                left += token.s;
                GET;
            }
            if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                left.erase(0, 1);
            } else {
                left += token.s;
            }
            GET;
            GET;
            while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                right += token.s;
                GET;
            }
            if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                right.erase(0, 1);
            } else {
                right += token.s;
            }
            auto leftAst = new AST(left, leftLine + baseLine);
            auto rightAst = new AST(right, rightLine + baseLine);
            leftAst->parse();
            rightAst->parse();
            root = new node(minOp.op, minOp.line + baseLine);
            root->insert(leftAst->root);
            root->insert(rightAst->root);
            delete leftAst;
            delete rightAst;
            break;
        }
    }
}

inline UL BM::AST::priority(const string& op) {
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