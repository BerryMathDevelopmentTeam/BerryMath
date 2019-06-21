#include <iostream>
#include <vector>
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
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.line()  + baseLine - 1);
                return;
            }
            root->insert(token.s, lexer.line());
            GET;
            if (token.t != Lexer::SET_TOKEN) {
                root->value("err");
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.line()  + baseLine - 1);
                return;
            }
            string expression("");
            do {
                GET;
                expression += token.s;
            } while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END);
            auto ast = new AST(expression, lexer.line()  + baseLine - 1);
            ast->parse();
            root->insert(ast->root);
            if (ast->root->value() == "bad-tree") {
                delete root;
                root = new node("bad-tree", ast->root->line());
                root->insert(ast->root->get(0));
                return;
            }
            delete ast;
            break;
        }
        case Lexer::NUMBER_TOKEN:
        case Lexer::STRING_TOKEN:
        case Lexer::BRACKETS_LEFT_TOKEN:
        case Lexer::UNKNOWN_TOKEN:
        {
            UL base = 1;
            struct Operator {
                UL pri;
                UL index;
                UL line;
                string op;
            };
            bool isUnknownTk = false;
            UL unknownTkLen = 0;
            if (token.t == Lexer::UNKNOWN_TOKEN) {
                isUnknownTk = true;
                unknownTkLen = token.s.length();
            }
            Operator minOp = {15, 0, 0, ""};
            const auto BRACKETS_PRI = priority("(");
            auto opIndex = lexer.i;
            auto leftLine = lexer.line(), rightLine = lexer.line();
            if (token.t == Lexer::BRACKETS_LEFT_TOKEN) base = BRACKETS_PRI;
            auto tmpToken = token;
            auto tmpTokenLine = lexer.line();
            auto tmpTokenIndex = lexer.index();
            GET;
            while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                if (token.t > Lexer::NOTE_TOKEN && token.t < Lexer::END_TOKEN) {// 是符号
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                        base *= BRACKETS_PRI;
                        if (isUnknownTk) {
                            auto pri = base;
                            if (pri < minOp.pri || minOp.op.empty()) {
                                minOp.pri = pri;
                                minOp.op = "call";
                                minOp.index = opIndex - unknownTkLen;
                                minOp.line = lexer.line();
                                rightLine = lexer.line();
                            }
                        }
                    }
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) base /= BRACKETS_PRI;
                    else {
                        auto pri = priority(token.s) * base;
                        if (pri < minOp.pri || minOp.op.empty()) {
                            minOp.pri = pri;
                            minOp.op = token.s;
                            minOp.index = opIndex;
                            minOp.line = lexer.line();
                            rightLine = lexer.line();
                        }
                    }
                }
                isUnknownTk = false;
                if (token.t == Lexer::UNKNOWN_TOKEN) {
                    isUnknownTk = true;
                    unknownTkLen = token.s.length();
                }
                opIndex = lexer.i;
                GET;
            }
            if (minOp.pri == 15 && minOp.index == 0 && minOp.line == 0 && minOp.op.empty()) {
                if (tmpToken.s == "(") {
                    lexer.i = tmpIndex;
                    lexer.get();
                    tmpToken = lexer.get();
                }
                root = new node(tmpToken.s, tmpTokenLine  + baseLine - 1);
                return;
            }
            string left, right;
            lexer.i = tmpIndex;
            GET;
            while (lexer.i < minOp.index) {
                left += token.s;
                GET;
            }
            if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                left.erase(0, 1);
            } else if (minOp.op != "call") {
                left += token.s;
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
                long long leftBCC = 0, rightBCC = 0;
                for (auto i = 0; i < left.length(); i++) {
                    if (left[i] == '(') leftBCC++;
                    else if (left[i] == ')') leftBCC--;
                }
                for (auto i = 0; i < right.length(); i++) {
                    if (right[i] == '(') rightBCC++;
                    else if (right[i] == ')') rightBCC--;
                }
                if (leftBCC > 0) for (auto i = 0; i < leftBCC; i++) left += ")";
                if (rightBCC < 0) {
                    rightBCC *= -1;
                    for (auto i = 0; i < rightBCC; i++) right = "(" + right;
                }
                auto leftAst = new AST(left, leftLine  + baseLine - 1);
                auto rightAst = new AST(right, rightLine  + baseLine - 1);
                leftAst->parse();
                if (leftAst->root->value() == "bad-tree") {
                    delete root;
                    root = new node("bad-tree", leftAst->root->line());
                    root->insert(leftAst->root->get(0));
                    return;
                }
                rightAst->parse();
                if (rightAst->root->value() == "bad-tree") {
                    delete root;
                    root = new node("bad-tree", rightAst->root->line());
                    root->insert(rightAst->root->get(0));
                    return;
                }
                root = new node(minOp.op, minOp.line  + baseLine - 1);
                root->insert(leftAst->root);
                root->insert(rightAst->root);
                delete leftAst;
                delete rightAst;
            } else {
                lexer.i = minOp.index;
                GET;
                UL funLine = lexer.l;
                string functionName(token.s);
                GET;
                UL brCount = 1;
                std::vector<string> args;
                string arg;
                while (brCount > 0) {
                    arg += token.s;
                    GET;
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) brCount++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) brCount--;
                    else if (token.t == Lexer::COMMA_TOKEN) {
                        arg.erase(0, 1);
                        args.push_back(arg);
                        arg = "";
                    } else if (token.t == Lexer::PROGRAM_END) {
                        root = new node("bad-tree", lexer.l + baseLine - 1);
                        root->insert("Lack of parentheses", lexer.l + baseLine - 1);
                        return;
                    }
                }
                if (!arg.empty()) {
                    arg.erase(0, 1);
                    args.push_back(arg);
                }
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    right += token.s;
                    GET;
                }
                auto callLine = minOp.line + baseLine;
                root = new node("call", callLine);
                root->insert(functionName, funLine  + baseLine - 1);
                root->insert("arg", funLine  + baseLine - 1);
                for (auto i = 0; i < args.size(); i++) {
                    auto ast = new AST(args[i], callLine);
                    ast->parse();
                    if (ast->root->value() == "bad-tree") {
                        delete root;
                        root = new node("bad-tree", ast->root->line());
                        root->insert(ast->root->get(0));
                        return;
                    }
                    root->get(1)->insert(ast->root);
                    delete ast;
                }
            }
            break;
        }
        case Lexer::IF_TOKEN:
        {
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine - 1);
                root->insert("Unexpected token " + token.s, lexer.l + baseLine - 1);
                return;
            }
            string ifExpression;
            auto bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine - 1);
                    root->insert("Lack of parentheses", lexer.l + baseLine - 1);
                    return;
                }
                ifExpression += token.s;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            ifExpression.erase(ifExpression.length() - 1, 1);
            std::cout << std::endl;
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