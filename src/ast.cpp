#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <tic.h>
#include "ast.h"

void BM::AST::parse() {
    // 解析cache内容
    if (byCache) {
        if (root) delete root;
        root = nullptr;
#define AGET token = astLexer.get()
        auto AGET;
        if (token.t == Lexer::PROGRAM_END) {
            root = new node("PROGRAM-END", astLexer.l + baseLine);
            return;
        }
        if (token.s != "node") {
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        AGET;
        string name(token.s);
        trim(name);
        UL findBase = 0;
        while (true) {
            auto i = name.find("\\", findBase);
            if (i == name.npos) break;
            name.erase(i, 1);
            while (i < name.length() && name[i] == '\\') i++;
            findBase = i;
        }
        AGET;
        if (token.s != "line") {
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        AGET;
        if (token.t != Lexer::NUMBER_TOKEN) {
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        std::stringstream ss;
        UL line;
        ss << token.s;
        ss >> line;
        AGET;
        if (token.s != "children") {
            delete root;
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        UL indentCount = 1;
        AGET;
        string childrenContent;
        root = new node(name, line);
        while (true) {
            if (token.s == "end") {
                if (--indentCount < 1) break;
                if (indentCount == 1) {
                    childrenContent += " " + token.s;
                    auto ast = new AST("", astLexer.l + baseLine);
                    ast->importByString(childrenContent);
                    ast->parse();
                    CHECK(ast);
                    CHANGELINES(ast);
                    root->insert(ast->root);
                    delete ast;
                    childrenContent = "";
                    AGET;
                    continue;
                }
            }
            else if (token.s == "children") indentCount++;
            childrenContent += " " + token.s;
            AGET;
        }
        return;
    }

    // 解析script内容
    if (!child && root) delete root;
    root = nullptr;
    auto tmpIndex = lexer.i;
#define GET token = lexer.get()
    auto GET;
    if (token.t == Lexer::NOTE_TOKEN) GET;
    switch (token.t) {
        case Lexer::LET_TOKEN:
        case Lexer::REFER_TOKEN:
        {
            if (token.t == Lexer::LET_TOKEN) root = new node("let", lexer.l + baseLine);
            else root = new node("refer", lexer.l + baseLine);
            GET;
            if (token.t != Lexer::UNKNOWN_TOKEN) {
                root->value("bad-tree");
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            root->insert(token.s, lexer.l + baseLine);
            GET;
            if (token.t != Lexer::SET_TOKEN && token.t != Lexer::END_TOKEN) {
                root->value("bad-tree");
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            string expression("");
            if (token.t == Lexer::END_TOKEN) {
                expression = "undefined";
            } else {
                UL sbc(0);
                UL mbc(0);
                UL bbc(0);
                do {
                    GET;
                    if (token.t == Lexer::PROGRAM_END) break;
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) sbc--;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbc--;
                    else if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                    else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bbc--;
                    expression += " " + token.s;
                    if (token.t == Lexer::END_TOKEN && sbc == 0 && mbc == 0 && bbc == 0) break;
                } while (true);
            }
            auto ast = new AST(expression, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            CHANGELINES(ast);
            root->insert(ast->root);
            delete ast;
            break;
        }
        case Lexer::NUMBER_TOKEN:
        case Lexer::STRING_TOKEN:
        case Lexer::BRACKETS_LEFT_TOKEN:
        case Lexer::UNKNOWN_TOKEN:
        case Lexer::UNDEFINED_TOKEN:
        case Lexer::NULL_TOKEN:
        {
            UL base = 1;
            struct Operator {
                UL pri;
                UL index;
                UL line;
                string op;
            };
            bool isUnknownTk = false;
            LL tempTK = 0;
            UL unknownTkLen = 0;
            if (token.t == Lexer::UNKNOWN_TOKEN || token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) {
                isUnknownTk = true;
                tempTK = lexer.sIndex;
                unknownTkLen = token.s.length();
            }
            Operator minOp = {15, 0, 0, ""};
            const auto BRACKETS_PRI = priority("(");
            const auto MBRACKETS_PRI = priority("[");
            auto opIndex = lexer.i;
            auto leftLine = lexer.l + baseLine, rightLine = lexer.l + baseLine;
            if (token.t == Lexer::BRACKETS_LEFT_TOKEN) base = BRACKETS_PRI;
            auto tmpToken = token;
            auto tmpTokenLine = lexer.l + baseLine;
            auto ig = false;
//            auto tmpTokenIndex = lexer.index();
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
                                minOp.index = tempTK;
                                minOp.line = lexer.l + baseLine;
                                rightLine = lexer.l + baseLine;
                            }
                        }
                    } else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                        base *= BRACKETS_PRI;
                        if (isUnknownTk) {
                            base /= BRACKETS_PRI;
                            base *= MBRACKETS_PRI;
                            auto pri = base;
                            if (pri < minOp.pri || minOp.op.empty()) {
                                minOp.pri = pri;
                                minOp.op = "get";
                                minOp.index = tempTK;
                                minOp.line = lexer.l + baseLine;
                                rightLine = lexer.l + baseLine;
                            }
                        }
                    }
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) base /= BRACKETS_PRI;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) base /= BRACKETS_PRI;
                    else {
                        auto pri = priority(token.s) * base;
                        if (pri < minOp.pri || minOp.op.empty()) {
                            minOp.pri = pri;
                            minOp.op = token.s;
                            minOp.index = opIndex;
                            minOp.line = lexer.l + baseLine;
                            rightLine = lexer.l + baseLine;
                        }
                    }
                }
                if (!isUnknownTk && token.t != Lexer::BRACKETS_LEFT_TOKEN && !ig) {
                    isUnknownTk = false;
                    tempTK = -1;
                }
                if (token.t == Lexer::UNKNOWN_TOKEN || token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) {
                    isUnknownTk = true;
                    unknownTkLen = token.s.length();
                    if (tempTK == -1) tempTK = opIndex;
                    ig = true;
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
                root = new node(tmpToken.s, tmpTokenLine);
                return;
            }
            string left, right;
            lexer.i = tmpIndex;
            GET;
            while (lexer.i < minOp.index) {
                left += " " + token.s;
                GET;
            }
            if (minOp.op == "call") {
                lexer.i = minOp.index;
                auto tmpFunNameIndex = lexer.i;
                string functionName("");
                GET;
                UL bcCount = 0;
                UL mbcCount = 0;
                while (true) {
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                        if (bcCount < 1 && mbcCount < 1) break;
                        bcCount++;
                    }
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbcCount++;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbcCount--;
                    functionName += token.s;
                    GET;
                }
                UL funLine = lexer.l + baseLine;
//                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
//                    lexer.i = tmpFunNameIndex;
//                    GET;
//                }
                GET;
                UL brCount = 1;
                std::vector<string> args;
                string arg;
                bool flag = false;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) brCount++;
                if (token.t != Lexer::BRACKETS_RIGHT_TOKEN) {
                    while (brCount > 0) {
                        arg += " " + token.s;
                        GET;
                        if (token.t == Lexer::BRACKETS_LEFT_TOKEN) brCount++;
                        else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) brCount--;
                        if ((token.t == Lexer::COMMA_TOKEN && brCount == 1) || !brCount) {
                            while (true) {
                                char n = arg[0];
                                if (n == ' ' || n == '\n' || n == '\t') arg.erase(0, 1);
                                else break;
                            }
                            while (true) {
                                char n = arg[arg.length() - 1];
                                if (n == ' ' || n == '\n' || n == '\t') arg.erase(arg.length() - 1, 1);
                                else break;
                            }
                            while (true) {
                                if (arg[0] == '(' && arg[arg.length() - 1] == ')') trim(arg);
                                else break;
                            }
                            if (flag) {
                                arg.erase(0, 2);
                                args.push_back(arg);
                            } else {
                                args.push_back(arg);
                                flag = true;
                            }
                            arg = "";
                        }
                        if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                            return;
                        }
                    }
                }
                if (!arg.empty()) {
                    if (flag) {
                        arg.erase(0, 2);
                    }
                    args.push_back(arg);
                }
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    right += " " + token.s;
                    GET;
                }
                auto callLine = minOp.line;
                root = new node("call", callLine);
                auto funNameAst = new AST(functionName, callLine);
                funNameAst->parse();
                CHECK(funNameAst);
                CHANGELINES(funNameAst);
                root->insert(funNameAst->root);
                root->insert("arg", funLine);
                for (auto i = 0; i < args.size(); i++) {
                    auto ast = new AST(args[i], callLine);
                    ast->parse();
                    CHECK(ast);
                    CHANGELINES(ast);
                    root->get(1)->insert(ast->root);
                    delete ast;
                }
                delete funNameAst;
            } else if (minOp.op == "get") {
                lexer.i = minOp.index;
                auto tmpGetNameIndex = lexer.i;
                GET;
                UL getLine = lexer.l + baseLine;
//                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
//                    lexer.i = tmpFunNameIndex;
//                    GET;
//                }
                root = new node("get", getLine);
                root->insert(token.s, lexer.l + baseLine);
                string getName(token.s);
                string expr;
                while (true) {
                    UL tempIndex = lexer.i;
                    GET;
                    if (token.t != Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                        lexer.i = tempIndex;
                        break;
                    }
                    UL bcCount = 1;
                    while (bcCount > 0) {
                        GET;
                        if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) bcCount++;
                        else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) {
                            bcCount--;
                            if (bcCount < 1) break;
                        }
                        expr += " " + token.s;
                    }
                    auto ast = new AST(expr, lexer.l + baseLine);
                    ast->parse();
                    CHECK(ast);
                    CHANGELINES(ast);
                    root->insert(ast->root);
                    expr = "";
                    delete ast;
                }
            } else {
                left += " " + token.s;
                GET;
                GET;
                LL sbc(0);
                LL mbc(0);
                LL bbc(0);
                while (sbc > 0 || mbc > 0 || bbc > 0 || (token.t != Lexer::PROGRAM_END && token.t != Lexer::END_TOKEN && token.t != Lexer::PASS_TOKEN)) {
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) sbc--;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbc--;
                    else if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                    else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bbc--;
                    right += " " + token.s;
                    GET;
                }
                if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    right.erase(0, 2);
                } else if (token.t != Lexer::PASS_TOKEN) {
                    right += " " + token.s;
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
                auto leftAst = new AST(left, leftLine);
                auto rightAst = new AST(right, rightLine);
                leftAst->parse();
                CHECK(leftAst);
                CHANGELINES(leftAst);
                rightAst->parse();
                CHECK(rightAst);
                CHANGELINES(rightAst);
                root = new node(minOp.op, minOp.line);
                if ((left == " " || left == " ()") && (minOp.op == "++" || minOp.op == "--" || minOp.op == "+" || minOp.op == "-")) {
                    if (minOp.op == "++" || minOp.op == "--")
                        root->value(minOp.op + "-f");
                    else root->insert("0", minOp.line);
                } else {
                    if (minOp.op == "++" || minOp.op == "--") {
                        root->insert(leftAst->root);
                    }
                    else root->insert(leftAst->root);
                }
                replace(right, " ");
                replace(right, "()");
                replace(right, "pass");
                replace(right, ";");
                if (right == "" && (minOp.op == "++" || minOp.op == "--")) {
                    root->value(minOp.op);
                } else {
                    root->insert(rightAst->root);
                }
                delete leftAst;
                delete rightAst;
            }
            break;
        }
        case Lexer::DADD_TOKEN:
        case Lexer::DSUB_TOKEN:
        {
            string op(token.s);
            root = new node(op + "-f", lexer.l + baseLine);
            string expr;
            while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END && token.t != Lexer::PASS_TOKEN) {
                GET;
                expr += token.s + " ";
            }
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            root->insert(ast->root);
            break;
        }
        case Lexer::MNOT_TOKEN:
        case Lexer::LNOT_TOKEN:
        {
            string op(token.s);
            root = new node(op, lexer.l + baseLine);
            GET;
            root->insert(token.s, lexer.l + baseLine);
            break;
        }
        case Lexer::SUB_TOKEN:
        case Lexer::ADD_TOKEN:
        {
            string op(token.s);
            root = new node(op, lexer.l + baseLine);
            GET;
            root->insert("0", lexer.l + baseLine);
            root->insert(token.s, lexer.l + baseLine);
            break;
        }
        case Lexer::IF_TOKEN:
        {
            auto ifLine = lexer.l + baseLine;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto exprLine = lexer.l + baseLine;
            string ifExpression;
            auto bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                ifExpression += " " + token.s;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            ifExpression.erase(ifExpression.length() - 1, 1);
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            string ifScript;
            bcCount = 1;
            UL ifScriptLine;
            GET;
            if (token.t == Lexer::PROGRAM_END) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                return;
            }
            ifScript += " " + token.s;
            if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
            else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            ifScriptLine = lexer.l + baseLine;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                ifScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            ifScript.erase(ifScript.length() - 1, 1);
            auto ifExprAst = new AST(ifExpression, exprLine);
            ifExprAst->parse();
            CHECK(ifExprAst);
            CHANGELINES(ifExprAst);
            root = new node("if", ifLine);
            root->insert(ifExprAst->root);
            root->insert(ifScript, ifScriptLine);
            root->insert("els", lexer.l + baseLine);
            delete ifExprAst;

            // elif, else等的解析
            UL tmpIndex;
            while (true) {
                tmpIndex = lexer.i;
                GET;
                if (token.t == Lexer::ELIF_TOKEN) {
                    GET;
                    if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    auto elExprLine = lexer.l + baseLine;
                    string elifExpression;
                    auto elbcCount = 1;
                    while (elbcCount > 0) {
                        GET;
                        if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                            return;
                        }
                        elifExpression += " " + token.s;
                        if (token.t == Lexer::BRACKETS_LEFT_TOKEN) elbcCount++;
                        else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) elbcCount--;
                    }
                    elifExpression.erase(elifExpression.length() - 1, 1);
                    GET;
                    if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }

                    string elifScript;
                    elbcCount = 1;
                    auto elifScriptLine = lexer.l + baseLine;
                    while (elbcCount > 0) {
                        GET;
                        if (token.t == Lexer::PROGRAM_END) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                            return;
                        }
                        elifScript += " " + token.s;
                        if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) elbcCount++;
                        else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) elbcCount--;
                    }
                    elifScript.erase(elifScript.length() - 1, 1);
                    auto elIfAst = new AST(elifExpression, elExprLine);
                    elIfAst->parse();
                    CHECK(elIfAst);
                    CHANGELINES(elIfAst);
                    root->get(2)->insert(new node("elif", elifScriptLine));
                    root->get(2)->get(-1)->insert(elIfAst->root);
                    root->get(2)->get(-1)->insert(elifScript, elifScriptLine);
                    delete elIfAst;
                } else if (token.t == Lexer::ELSE_TOKEN) {
                    GET;
                    UL elBcCount = 1;
                    if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    string elScript;
                    auto elScriptLine = lexer.l + baseLine;
                    while (elBcCount > 0) {
                        GET;
                        if (token.t == Lexer::PROGRAM_END) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                            return;
                        }
                        elScript += " " + token.s;
                        if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) elBcCount++;
                        else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) elBcCount--;
                    }
                    elScript.erase(elScript.length() - 1, 1);
                    root->get(2)->insert("else", elScriptLine);
                    root->get(2)->get(-1)->insert(elScript,  elScriptLine);
                    break;
                } else {
                    lexer.i = tmpIndex;
                    break;
                }
            }
            break;
        }
        case Lexer::SWITCH_TOKEN:
        {
            auto switchLine = lexer.l + baseLine;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto bcCount = 1;
            string switchExpr;
            GET;
            while (bcCount > 0) {
                switchExpr += " " + token.s;
                GET;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
                else if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
            }
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            bcCount = 1;
            string block;
            GET;
            if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
            else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            else if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                return;
            }
            while (bcCount > 0) {
                block += " " + token.s;
                GET;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
                else if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
            }
            auto rawLexerI = lexer.i;
            auto rawLexerL = lexer.l;
            lexer.open(block);
            GET;
            root = new node("switch", switchLine);
            while (token.t != Lexer::PROGRAM_END) {
                if (token.t == Lexer::CASE_TOKEN) {
                    auto caseLine = lexer.l + baseLine;
                    string caseExpr;
                    GET;
                    while (token.t != Lexer::COLON_TOKEN) {
                        caseExpr += " " + token.s;
                        GET;
                    }
                    bcCount = 0;
                    string script;
                    auto lastI = lexer.i;
                    GET;
                    while (true) {
                        if ((token.t == Lexer::DEFAULT_TOKEN || token.t == Lexer::CASE_TOKEN) && bcCount < 1) break;
                        script += " " + token.s;
                        lastI = lexer.i;
                        GET;
                    }
                    root->insert("case", caseLine);
                    auto ast = new AST(caseExpr, caseLine);
                    ast->parse();
                    CHECK(ast);
                    CHANGELINES(ast);
                    root->get(-1)->insert(ast->root);
                    root->get(-1)->insert(script, lexer.l + baseLine);
                    delete ast;

                    lexer.i = lastI;
                } else if (token.t == Lexer::DEFAULT_TOKEN) {
                    auto defaultLine = lexer.l + baseLine;
                    GET;
                    if (token.t != Lexer::COLON_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    string script;
                    GET;
                    while (token.t != Lexer::PROGRAM_END) {
                        script += " " + token.s;
                        GET;
                    }
                    root->insert("default", defaultLine);
                    root->get(-1)->insert(script, lexer.l + baseLine);
                } else {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                    return;
                }
                GET;
            }

            // come back to raw
            lexer.open(script);
            lexer.i = rawLexerI;
            lexer.l = rawLexerL;
            break;
        }
        case Lexer::WHILE_TOKEN:
        {
            auto whileLine = lexer.l + baseLine;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto exprLine = lexer.l + baseLine;
            string whileExpression;
            auto bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                whileExpression += " " + token.s;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            whileExpression.erase(whileExpression.length() - 1, 1);

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            string whileScript;
            bcCount = 1;
            auto whileScriptLine = lexer.l + baseLine;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                whileScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            whileScript.erase(whileScript.length() - 1, 1);
            root = new node("while", whileLine);
            auto exprAst = new AST(whileExpression, exprLine);
            exprAst->parse();
            CHECK(exprAst);
            CHANGELINES(exprAst);
            root->insert(exprAst->root);
            root->insert(whileScript, whileScriptLine);

            delete exprAst;
            break;
        }
        case Lexer::DO_TOKEN:
        {
            auto doLine = lexer.l + baseLine;

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            string whileScript;
            auto bcCount = 1;
            auto whileScriptLine = lexer.l + baseLine;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                whileScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            whileScript.erase(whileScript.length() - 1, 1);

            GET;
            if (token.t != Lexer::WHILE_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto exprLine = lexer.l + baseLine;
            string whileExpression;
            bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                whileExpression += " " + token.s;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            whileExpression.erase(whileExpression.length() - 1, 1);
            root = new node("do", doLine);
            auto exprAst = new AST(whileExpression, exprLine);
            exprAst->parse();
            CHECK(exprAst);
            CHANGELINES(exprAst);
            root->insert(whileScript, whileScriptLine);
            root->insert(exprAst->root);
            break;
        }
        case Lexer::FOR_TOKEN:
        {
            auto forLine = lexer.l + baseLine;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto bcCount = 1;
            string forExprScript;
            while (true) {
                GET;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    if (--bcCount < 1) break;
                }
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                forExprScript += " " + token.s;
            }
            auto ch = forExprScript[forExprScript.length() - 1];
            if (ch == ' ' || ch == '\t' || ch == '\n') forExprScript.erase(forExprScript.length() - 1, 1);

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            string forScript;
            bcCount = 1;
            auto forScriptLine = lexer.l + baseLine;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                forScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            forScript.erase(forScript.length() - 1, 1);

            root = new node("for", forLine);
            auto ast = new AST(forExprScript, forLine);
            ast->parse();
            CHECK(ast);
            CHANGELINES(ast);
            root->insert(ast->root);
            if (ast->root->value() != "in" && ast->root->value() != "of") {
                ast->parse();
                CHECK(ast);
                CHANGELINES(ast);
                root->insert(ast->root);
                ast->parse();
                CHECK(ast);
                CHANGELINES(ast);
                root->insert(ast->root);
            }
            root->insert(forScript, forScriptLine);

            delete ast;
            break;
        }
        case Lexer::DEF_TOKEN:
        case Lexer::STATIC_TOKEN:// static function
        {
            bool isStatic = token.t == Lexer::STATIC_TOKEN;
            auto defLine = lexer.l + baseLine;

            GET;
            string funcName(token.s);
            if (token.t != Lexer::UNKNOWN_TOKEN) {
                funcName = "";
            } else {
                GET;
            }

            enum ArgType {
                DEFAULT, REFER
            };
            struct Arg {
                ArgType type;
                string arg;
                string defaultValue = "";
                Arg(ArgType t, string a, string dv = "") : type(t), arg(a), defaultValue(dv) { }
            };

            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                funcName += token.s;
                GET;
                if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                    GET;
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                    return;
                }
            }
            string arg;
            vector<Arg> args;
            auto bcCount = 1;
            while (true) {
                GET;
                if (token.t == Lexer::COMMA_TOKEN) {
                    Lexer l(arg);
                    auto t = l.get();
                    auto tmpI = l.i;
                    string defaultValue("");
                    if (t.t == Lexer::DEFAULT_TOKEN) {
                        auto name = l.get().s;
                        auto t2 = l.get();
                        if (t2.t == Lexer::SET_TOKEN) {
                            t2 = l.get();
                            while (t2.t != Lexer::PROGRAM_END) {
                                defaultValue += " " + t2.s;
                                t2 = l.get();
                            }
                        }
                        args.push_back(Arg(DEFAULT, name, defaultValue));
                    } else if (t.t == Lexer::UNKNOWN_TOKEN) {
                        auto t2 = l.get();
                        if (t2.t == Lexer::SET_TOKEN) {
                            t2 = l.get();
                            while (t2.t != Lexer::PROGRAM_END) {
                                defaultValue += " " + t2.s;
                                t2 = l.get();
                            }
                        }
                        args.push_back(Arg(DEFAULT, t.s, defaultValue));
                    } else if (t.t == Lexer::REFER_TOKEN) {
                        auto name = l.get().s;
                        auto t2 = l.get();
                        if (t2.t == Lexer::SET_TOKEN) {
                            t2 = l.get();
                            while (t2.t != Lexer::PROGRAM_END) {
                                defaultValue += " " + t2.s;
                                t2 = l.get();
                            }
                        }
                        args.push_back(Arg(REFER, name, defaultValue));
                    } else {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    arg = "";
                    continue;
                } else if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                    bcCount++;
                } else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    if (--bcCount < 1) break;
                }
                arg += " " + token.s;
            }
            Lexer l(arg);
            auto t = l.get();
            string defaultValue("");
            if (t.t == Lexer::DEFAULT_TOKEN) {
                auto name = l.get().s;
                auto t2 = l.get();
                if (t2.t == Lexer::SET_TOKEN) {
                    t2 = l.get();
                    while (t2.t != Lexer::PROGRAM_END) {
                        defaultValue += " " + t2.s;
                        t2 = l.get();
                    }
                }
                args.push_back(Arg(DEFAULT, name, defaultValue));
            } else if (t.t == Lexer::UNKNOWN_TOKEN) {
                auto t2 = l.get();
                if (t2.t == Lexer::SET_TOKEN) {
                    t2 = l.get();
                    while (t2.t != Lexer::PROGRAM_END) {
                        defaultValue += " " + t2.s;
                        t2 = l.get();
                    }
                }
                args.push_back(Arg(DEFAULT, t.s, defaultValue));
            } else if (t.t == Lexer::REFER_TOKEN) {
                auto name = l.get().s;
                auto t2 = l.get();
                if (t2.t == Lexer::SET_TOKEN) {
                    t2 = l.get();
                    while (t2.t != Lexer::PROGRAM_END) {
                        defaultValue += " " + t2.s;
                        t2 = l.get();
                    }
                }
                args.push_back(Arg(REFER, name, defaultValue));
            }

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            string funcScript;
            bcCount = 1;
            auto funcScriptLine = lexer.l + baseLine;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                funcScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            funcScript.erase(funcScript.length() - 1, 1);

            auto dl = defLine;
            if (isStatic) root = new node("static", dl);
            else root = new node("def", dl);
            root->insert(funcName, dl);
            root->insert("args", dl);
            for (auto i = 0; i < args.size(); i++) {
                Arg& a = args[i];
                root->get(1)->insert(a.arg, dl);
                if (a.type == REFER) root->get(1)->get(-1)->insert("refer", dl);
                else root->get(1)->get(-1)->insert("default", dl);
                auto ast = new AST(a.defaultValue, dl);
                ast->parse();
                CHECK(ast);
                CHANGELINES(ast);
                root->get(1)->get(-1)->insert(ast->root);
                delete ast;
            }
            root->insert(funcScript, funcScriptLine);
            break;
        }
        case Lexer::RETURN_TOKEN:
        {
            ULL line = lexer.l + baseLine;
            LL sbc(0), mbc(0), bbc(0);
            string ret;
            while (true) {
                GET;
                if (token.t == Lexer::END_TOKEN && sbc == 0 && mbc == 0 && bbc == 0) break;
                if (token.t == Lexer::PROGRAM_END && (sbc > 0 || mbc > 0 || bbc > 0)) {
                    if (root) delete root;
                    root = new node("bad-tree", line);
                    root->insert("SyntaxError: The unexpected end of the script", line);
                    return;
                }
                ret += " " + token.s;
            }
            if (ret.empty()) ret = "undefined";
            if (root) delete root;
            root = new node("return", line);
            root->insert(ret, line);
            break;
        }
        case Lexer::USING_TOKEN:
        {
            UL usingLine = lexer.l + baseLine;
            string expr;
            GET;
            while (token.t != Lexer::END_TOKEN) {
                expr += token.s + " ";
                GET;
            }
            root = new node("using", usingLine);
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            CHANGELINES(ast);
            root->insert(ast->root);
            delete ast;
            break;
        }
        case Lexer::BREAK_TOKEN:
        {
            UL breakLine = lexer.l + baseLine;
            string expr;
            GET;
            while (token.t != Lexer::END_TOKEN) {
                expr += token.s + " ";
                GET;
            }
            root = new node("break", breakLine);
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            CHANGELINES(ast);
            if (ast->root->value() != "undefined") root->insert(ast->root);
            else {
                root->insert(new node("1", lexer.l + baseLine));
                delete ast->root;
            }
            delete ast;
            break;
        }
        case Lexer::CONTINUE_TOKEN:
        {
            root = new node("continue", lexer.l + baseLine);
            break;
        }
        case Lexer::IMPORT_TOKEN:
        {
            root = new node("import", lexer.l + baseLine);
            string expr;
            UL bcCount = 0;
            GET;
            bool hasAs(false);
            while (token.t != Lexer::END_TOKEN || bcCount > 0) {
                if (token.t == Lexer::AS_TOKEN && bcCount < 1) {
                    hasAs = true;
                    break;
                }
                expr += " " + token.s;
                GET;
            }
            string asName("module");
            if (hasAs) {
                GET;
                asName = token.s;
            } else {
                expr.erase(0, 1);
                asName = expr;
                expr.insert(0, "\"");
                expr.insert(expr.length(), "\"");
            }
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            CHANGELINES(ast);
            root->insert(ast->root);
            root->insert("as", lexer.l + baseLine);
            root->get(-1)->insert(asName, lexer.l + baseLine);
            delete ast;
            break;
        }
        case Lexer::EXPORT_TOKEN:
        {
            root = new node("export", lexer.l + baseLine);
            GET;
            root->insert(token.s, lexer.l + baseLine);
            break;
        }
        case Lexer::DELETE_TOKEN:
        {
            root = new node("delete", lexer.l + baseLine);
            GET;
            string expr;
            while (token.t != Lexer::END_TOKEN) {
                expr += " " + token.s;
                GET;
            }
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            CHANGELINES(ast);
            root->insert(ast->root);
            delete ast;
            break;
        }
        case Lexer::PASS_TOKEN:
        {
            root = new node("pass", lexer.l + baseLine);
            break;
        }
        case Lexer::PROGRAM_END:
        {
            root = new node("PROGRAM-END", lexer.l + baseLine);
            break;
        }
        case Lexer::BIG_BRACKETS_LEFT_TOKEN:
        {
            auto upLine = lexer.line();
            auto upIndex = lexer.index();
            GET;
            GET;
            lexer.i = upIndex;
            lexer.l = upLine;
            if (token.t == Lexer::COLON_TOKEN) {
                root = new node("o-value", upLine + baseLine);
                // 是一个Object value
                LL sbc(0);
                LL mbc(0);
                LL bbc(1);
                while (true) {
                    GET;
                    if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) break;
                    if (token.t > Lexer::STRING_TOKEN) {
                        delete root;
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Wrong object key: '" + token.s + "'", lexer.l + baseLine);
                        return;
                    }
                    string key(token.s);
                    if (token.t == Lexer::STRING_TOKEN) {
                        key.erase(key.begin(), key.begin() + 1);
                        key.erase(key.end() - 1, key.end());
                    }
                    root->insert(key, lexer.l + baseLine);
                    GET;
                    if (token.t != Lexer::COLON_TOKEN) {
                        delete root;
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token: '" + token.s + "'", lexer.l + baseLine);
                        return;
                    }
                    bool finish = false;
                    string valueStr;
                    while (true) {
                        GET;
                        if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                        else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bbc--;
                        else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                        else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbc--;
                        else if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                        else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) sbc--;
                        if (token.t == Lexer::COMMA_TOKEN && bbc == 1 && !mbc && !sbc) {
                            break;
                        }
                        if (!bbc && !mbc && !sbc) {
                            finish = true;
                            break;
                        }
                        if (token.t == Lexer::PROGRAM_END) {
                            delete root;
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("SyntaxError: Lack of commas", lexer.l + baseLine);
                            return;
                        }
                        valueStr += token.s + " ";
                    }
                    root->get(-1)->insert(valueStr, lexer.l + baseLine);
                    if (finish) break;
                }
            } else {
                GET;
                if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    // 是对象
                    root = new node("o-value", upLine + baseLine);
                } else {
                    lexer.i = upIndex;
                    lexer.l = upLine;
                    // 是局部作用域声明
                    LL bbc(1);
                    string script;
                    while (true) {
                        GET;
                        if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                        else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                            bbc--;
                            if (!bbc) break;
                        }
                        script += token.s + " ";
                    }
                    root = new node("if", lexer.l + baseLine);// 就是将{...}这样的局部作用域声明改为if (1) {...}这样的ast
                    root->insert("1", lexer.l + baseLine);
                    root->insert(script, lexer.l + baseLine);
                    root->insert("els", lexer.l + baseLine);
                }
            }
            break;
        }
        case Lexer::MIDDLE_BRACKETS_LEFT_TOKEN:
        {
            LL mbc(1);
            string strValue;
            root = new node("a-value", lexer.l + baseLine);
            while (true) {
                GET;
                if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN)if (!(--mbc)) break;
                if (token.t == Lexer::COMMA_TOKEN && mbc == 1) {
                    root->insert(strValue, lexer.l + baseLine);
                    strValue = "";
                } else if (token.t == Lexer::PROGRAM_END) {
                    if (mbc > 0) {
                        root->insert("SyntaxError: Lack of middle brackets", lexer.l + baseLine);
                    }
                } else {
                    strValue += token.s + " ";
                }
            }
            if (!strValue.empty()) {
                root->insert(strValue, lexer.l + baseLine);
            }
            break;
        }
        case Lexer::CLASS_TOKEN:
        {
            auto ClassDefLine = lexer.l + baseLine;
            root = new node("class", ClassDefLine);
            GET;
            string className(token.s);
            root->insert(className, lexer.l + baseLine);
            GET;
            vector<string> extends;
            if (token.t == Lexer::EXTENDS_TOKEN) {
                string extendClassName;
                while (true) {
                    GET;
                    if (token.t == Lexer::COMMA_TOKEN) continue;
                    if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) break;
                    extendClassName += token.s;
                }
            }
            bool pflag(false);// private flag

            LL bbc(1);
            while (true) {
                GET;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    bbc--;
                    if (bbc < 1) break;
                }
                if (token.t == Lexer::PRIVATE_TOKEN) {
                    GET;
                    if (token.t != Lexer::COLON_TOKEN) {
                        delete root;
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token '" + token.s + "'", lexer.l + baseLine);
                    }
                    pflag = true;
                } else if (token.t == Lexer::PUBLIC_TOKEN) {
                    GET;
                    if (token.t != Lexer::COLON_TOKEN) {
                        delete root;
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token '" + token.s + "'", lexer.l + baseLine);
                    }
                    pflag = false;
                } else if (token.t == Lexer::NOTE_TOKEN) {
                    // 与pass一样，直接跳过
                } else {
                    if (token.t == Lexer::DEF_TOKEN || token.t == Lexer::STATIC_TOKEN || token.t == Lexer::OPERATOR_TOKEN) {
                        string defScript;
                        auto def_token_ref = token;
                        if (token.t == Lexer::DEF_TOKEN || token.t == Lexer::OPERATOR_TOKEN) defScript = "def ";
                        else defScript = "static ";
                        GET;
                        if (def_token_ref.t == Lexer::OPERATOR_TOKEN) defScript += " " + OPERATOR_PROTO_NAME + token.s;
                        else defScript += " " + token.s;
                        GET;
                        defScript += " " + token.s;
                        UL sbc(1);
                        while (true) {
                            GET;
                            defScript += " " + token.s;
                            if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                            else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                                sbc--;
                                if (sbc < 1) break;
                            }
                        }
                        GET;
                        defScript += " " +  token.s;
                        UL bbc(1);
                        while (true) {
                            GET;
                            defScript += " " + token.s;
                            if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                            else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                                bbc--;
                                if (bbc < 1) break;
                            }
                        }
                        auto ast = new AST(defScript, lexer.l + baseLine);
                        ast->parse();
                        CHECK(ast);
                        root->get(0)->insert(ast->root->get(0)->value(), lexer.l + baseLine);
                        if (pflag) {
                            root->get(0)->get(-1)->insert("private", lexer.l + baseLine);
                        } else {
                            root->get(0)->get(-1)->insert("public", lexer.l + baseLine);
                        }
                        root->get(0)->get(-1)->insert(ast->root);
                    } else if (token.t == Lexer::UNKNOWN_TOKEN) {
                        root->get(0)->insert(token.s, lexer.l + baseLine);
                        if (pflag) {
                            root->get(0)->get(-1)->insert("private", lexer.l + baseLine);
                        } else {
                            root->get(0)->get(-1)->insert("public", lexer.l + baseLine);
                        }
                        GET;
                        if (token.t == Lexer::SET_TOKEN) {
                            string expression("");
                            UL sbc(0);
                            UL mbc(0);
                            UL bbc(0);
                            do {
                                GET;
                                if (token.t == Lexer::PROGRAM_END) break;
                                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) sbc--;
                                else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                                else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbc--;
                                else if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bbc--;
                                expression += " " + token.s;
                                if (token.t == Lexer::END_TOKEN && sbc == 0 && mbc == 0 && bbc == 0) break;
                            } while (true);
                            auto ast = new AST(expression, lexer.l + baseLine);
                            ast->parse();
                            CHECK(ast);
                            root->get(0)->get(-1)->insert(ast->root);
                        } else {
                            root->get(0)->get(-1)->insert("undefined", lexer.l + baseLine);
                        }
                    } else if (token.t != Lexer::BIG_BRACKETS_RIGHT_TOKEN && token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                        delete root;
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("SyntaxError: Unexpected token '" + token.s + "'", lexer.l + baseLine);
                        return;
                    }
                }
            }
            root->insert("extends", ClassDefLine);
            auto node = root->get(-1);
            for (auto i = extends.begin(); i != extends.end(); i++) {
                node->insert(*i, ClassDefLine);
            }
            break;
        }
        case Lexer::NEW_TOKEN:
        {
            GET;
            string expression(token.s + " ");
            UL sbc(0);
            UL mbc(0);
            UL bbc(0);
            do {
                GET;
                if (token.t == Lexer::PROGRAM_END) break;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) sbc--;
                else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbc--;
                else if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bbc--;
                expression += " " + token.s;
                if (token.t == Lexer::END_TOKEN && sbc == 0 && mbc == 0 && bbc == 0) break;
            } while (true);
            auto ast = new AST(expression, lexer.l + baseLine);
            ast->parse();
            root = ast->root;
            root->value("new");
            break;
        }
    }
    if (!root) root = new node("pass", lexer.l + baseLine);
}
string BM::AST::exportByString() {
    if (!root) return "null";
    return root->exportByString();
}
bool BM::AST::Export(string filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file) return false;
    file << exportByString();
    file.close();
    return true;
}
string BM::AST::node::exportByString() {
    auto s = value();
    std::regex p1("\'(.)");
    std::regex p2("\"(.)");
    s = std::regex_replace(s, p1, "\\\'$1");
    s = std::regex_replace(s, p2, "\\\"$1");
    string res("node \"" + s + "\" line " + std::to_string(line()) + " children ");
    for (UL i = 0; i < children.size(); i++) {
        res += children[i]->exportByString();
    }
    res += " end ";
    return res;
}

void BM::AST::importByString(string s) {
    byCache = true;
    astLexer.open(s);
}
void BM::AST::import(string filename) {
    std::ifstream file(filename);
    string tmp;
    string s;
    while (getline(file, tmp)) {
        s += tmp + "\n";
    }
    file.close();
    importByString(s);
}
BM::AST::~AST() {
    if (!child && root)
        delete root;
}

inline UL BM::AST::priority(const string& op) {
    if (op == "[" || op == ".") return 15;
    if (op == "(") return 14;
    if (op == "++" || op == "--" || op == "!" || op == "~" || op == "**") return 13;
    if (op == "*" || op == "/" || op == "%") return 12;
    if (op == "+" || op == "-") return 11;
    if (op == "<<" || op == ">>") return 10;
    if (op == ">" || op == ">=" || op == "<" || op == "<=") return 9;
    if (op == "==" || op == "!=") return 8;
    if (op == "&") return 7;
    if (op == "^") return 6;
    if (op == "|") return 5;
    if (op == "&&") return 4;
    if (op == "||" || op == "~~") return 3;
    if (
            op == "="
            || op == "*=" || op == "/=" || op == "%="
            || op == "+=" || op == "-="
            || op == ">>=" || op == "<<="
            || op == "^=" || op == "&=" || op == "|="
            || op == "in" || op == "of"
            )
        return 2;
    if (op == ",") return 1;
    return 15;
}