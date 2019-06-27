#include <iostream>
#include <vector>
#include "ast.h"

void BM::AST::parse() {
    auto tmpIndex = lexer.i;
    auto token = lexer.get();
#define GET token = lexer.get()

    switch (token.t) {
        case Lexer::LET_TOKEN:
        {
            root = new node("let", lexer.l + baseLine);// 因为行数起步都是1, 所以如果不减1, 就会导致最后行数错位, 多1
            GET;
            if (token.t != Lexer::UNKNOWN_TOKEN) {
                root->value("bad-tree");
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l  + baseLine);
                return;
            }
            root->insert(token.s, lexer.l);
            GET;
            if (token.t != Lexer::SET_TOKEN) {
                root->value("bad-tree");
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l  + baseLine);
                return;
            }
            string expression("");
            do {
                GET;
                expression += " " + token.s;
            } while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END);
            auto ast = new AST(expression, lexer.l  + baseLine);
            ast->parse();
            root->insert(ast->root);
            CHECK(ast);
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
            UL unknownTkLen = 0;
            if (token.t == Lexer::UNKNOWN_TOKEN) {
                isUnknownTk = true;
                unknownTkLen = token.s.length();
            }
            Operator minOp = {15, 0, 0, ""};
            const auto BRACKETS_PRI = priority("(");
            auto opIndex = lexer.i;
            auto leftLine = lexer.l, rightLine = lexer.l;
            if (token.t == Lexer::BRACKETS_LEFT_TOKEN) base = BRACKETS_PRI;
            auto tmpToken = token;
            auto tmpTokenLine = lexer.l;
//            auto tmpTokenIndex = lexer.index();
            GET;
            UL tempTK = 0;
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
                                minOp.line = lexer.l;
                                rightLine = lexer.l;
                            }
                        }
                    } else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                        base *= BRACKETS_PRI;
                        if (isUnknownTk) {
                            auto pri = base;
                            if (pri < minOp.pri || minOp.op.empty()) {
                                minOp.pri = pri;
                                minOp.op = "get";
                                minOp.index = tempTK;
                                minOp.line = lexer.l;
                                rightLine = lexer.l;
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
                            minOp.line = lexer.l;
                            rightLine = lexer.l;
                        }
                    }
                }
                isUnknownTk = false;
                if (token.t == Lexer::UNKNOWN_TOKEN) {
                    isUnknownTk = true;
                    unknownTkLen = token.s.length();
                    tempTK = opIndex;
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
                root = new node(tmpToken.s, tmpTokenLine  + baseLine);
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
                GET;
                UL funLine = lexer.l;
//                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
//                    lexer.i = tmpFunNameIndex;
//                    GET;
//                }
                string functionName(token.s);
                GET;
                UL brCount = 1;
                std::vector<string> args;
                string arg;
                while (brCount > 0) {
                    arg += " " + token.s;
                    GET;
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) brCount++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) brCount--;
                    else if (token.t == Lexer::COMMA_TOKEN) {
                        arg.erase(0, 2);
                        args.push_back(arg);
                        arg = "";
                    } else if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                        return;
                    }
                }
                if (!arg.empty()) {
                    arg.erase(0, 2);
                    args.push_back(arg);
                }
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    right += " " + token.s;
                    GET;
                }
                auto callLine = minOp.line + baseLine;
                root = new node("call", callLine);
                root->insert(functionName, funLine  + baseLine);
                root->insert("arg", funLine  + baseLine);
                for (auto i = 0; i < args.size(); i++) {
                    auto ast = new AST(args[i], callLine);
                    ast->parse();
                    CHECK(ast);
                    root->get(1)->insert(ast->root);
                    delete ast;
                }
            } else if (minOp.op == "get") {
                lexer.i = minOp.index;
                auto tmpGetNameIndex = lexer.i;
                GET;
                UL getLine = lexer.l;
//                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
//                    lexer.i = tmpFunNameIndex;
//                    GET;
//                }
                root = new node("get", getLine);
                root->insert(token.s, lexer.l);
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
                    root->insert(ast->root);
                    expr = "";
                    delete ast;
                }
            } else {
                left += " " + token.s;
                GET;
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    right += " " + token.s;
                    GET;
                }
                if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    right.erase(0, 2);
                } else {
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
                auto leftAst = new AST(left, leftLine + baseLine);
                auto rightAst = new AST(right, rightLine + baseLine);
                leftAst->parse();
                if (leftAst->root->value() == "bad-tree") {
                    delete root;
                    root = leftAst->root;
                    delete leftAst;
                    delete rightAst;
                    return;
                }
                rightAst->parse();
                if (rightAst->root->value() == "bad-tree") {
                    delete root;
                    root = rightAst->root;
                    delete leftAst;
                    delete rightAst;
                    return;
                }
                root = new node(minOp.op, minOp.line + baseLine);
                if (!((left == " " || left == " ()") && (minOp.op == "++" || minOp.op == "--"))) {
                    root->insert(leftAst->root);
                } else {
                    root->value("a" + minOp.op);
                }
                if (!((right == " " || right == " ()") && (minOp.op == "++" || minOp.op == "--"))) {
                    root->insert(rightAst->root);
                } else {
                    root->value("f" + minOp.op);
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
            root = new node(op, lexer.l);
            GET;
            root->insert("f-" + token.s, lexer.l);
            break;
        }
        case Lexer::IF_TOKEN:
        {
            auto ifLine = lexer.l;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto exprLine = lexer.l;
            string ifExpression;
            auto bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
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
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            string ifScript;
            bcCount = 1;
            auto ifScriptLine = lexer.l;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                ifScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            ifScript.erase(ifScript.length() - 1, 1);
            auto ifExprAst = new AST(ifExpression, exprLine + baseLine);
            ifExprAst->parse();
            CHECK(ifExprAst);
            root = new node("if", ifLine + baseLine);
            root->insert(ifExprAst->root);
            root->insert(ifScript, ifScriptLine + baseLine);
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
                        root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    auto elExprLine = lexer.l;
                    string elifExpression;
                    auto elbcCount = 1;
                    while (elbcCount > 0) {
                        GET;
                        if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
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
                        root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }

                    string elifScript;
                    elbcCount = 1;
                    auto elifScriptLine = lexer.l;
                    while (elbcCount > 0) {
                        GET;
                        if (token.t == Lexer::PROGRAM_END) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                            return;
                        }
                        elifScript += " " + token.s;
                        if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) elbcCount++;
                        else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) elbcCount--;
                    }
                    elifScript.erase(elifScript.length() - 1, 1);
                    auto elIfAst = new AST(elifExpression, elExprLine + baseLine);
                    elIfAst->parse();
                    CHECK(elIfAst);
                    root->get(2)->insert(new node("elif", elifScriptLine + baseLine));
                    root->get(2)->get(-1)->insert(elIfAst->root);
                    root->get(2)->get(-1)->insert(elifScript, elifScriptLine + baseLine);
                    delete elIfAst;
                } else if (token.t == Lexer::ELSE_TOKEN) {
                    GET;
                    UL elBcCount = 1;
                    if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    string elScript;
                    auto elScriptLine = lexer.l;
                    while (elBcCount > 0) {
                        GET;
                        if (token.t == Lexer::PROGRAM_END) {
                            root = new node("bad-tree", lexer.l + baseLine);
                            root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                            return;
                        }
                        elScript += " " + token.s;
                        if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) elBcCount++;
                        else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) elBcCount--;
                    }
                    elScript.erase(elScript.length() - 1, 1);
                    root->get(2)->insert("else", elScriptLine + baseLine);
                    root->get(2)->get(-1)->insert(elScript,  elScriptLine + baseLine);
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
            auto switchLine = lexer.l;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
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
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
            }
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            bcCount = 1;
            string block;
            GET;
            if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
            else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            else if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                return;
            }
            while (bcCount > 0) {
                block += " " + token.s;
                GET;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
                else if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
            }
            auto rawLexerI = lexer.i;
            auto rawLexerL = lexer.l;
            lexer.open(block);
            GET;
            root = new node("switch", rawLexerL );
            while (token.t != Lexer::PROGRAM_END) {
                if (token.t == Lexer::CASE_TOKEN) {
                    auto caseLine = lexer.l;
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
                    root->insert("case", caseLine + baseLine);
                    auto ast = new AST(caseExpr, caseLine + baseLine);
                    ast->parse();
                    CHECK(ast);
                    root->get(-1)->insert(ast->root);
                    root->get(-1)->insert(script, lexer.l + baseLine);
                    delete ast;

                    lexer.i = lastI;
                } else if (token.t == Lexer::DEFAULT_TOKEN) {
                    auto defaultLine = lexer.l;
                    GET;
                    if (token.t != Lexer::COLON_TOKEN) {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                        return;
                    }
                    string script;
                    GET;
                    while (token.t != Lexer::PROGRAM_END) {
                        script += " " + token.s;
                        GET;
                    }
                    root->insert("default", defaultLine + baseLine);
                    root->get(-1)->insert(script, lexer.l + baseLine);
                } else {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
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
            auto whileLine = lexer.l;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto exprLine = lexer.l;
            string whileExpression;
            auto bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
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
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            string whileScript;
            bcCount = 1;
            auto whileScriptLine = lexer.l;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                whileScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            whileScript.erase(whileScript.length() - 1, 1);
            root = new node("while", whileLine + baseLine);
            auto exprAst = new AST(whileExpression, exprLine + baseLine);
            exprAst->parse();
            CHECK(exprAst);
            root->insert(exprAst->root);
            root->insert(whileScript, whileScriptLine);

            delete exprAst;
            break;
        }
        case Lexer::DO_TOKEN:
        {
            auto doLine = lexer.l;

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            string whileScript;
            auto bcCount = 1;
            auto whileScriptLine = lexer.l;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
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
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto exprLine = lexer.l;
            string whileExpression;
            bcCount = 1;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                whileExpression += " " + token.s;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            whileExpression.erase(whileExpression.length() - 1, 1);
            root = new node("do", doLine + baseLine);
            auto exprAst = new AST(whileExpression, exprLine + baseLine);
            exprAst->parse();
            CHECK(exprAst);
            root->insert(whileScript, whileScriptLine);
            root->insert(exprAst->root);
            break;
        }
        case Lexer::FOR_TOKEN:
        {
            auto forLine = lexer.l;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            auto bcCount = 1;
            string forExprScript;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                forExprScript += " " + token.s;
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            forExprScript.erase(forExprScript.length() - 1, 1);

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            string forScript;
            bcCount = 1;
            auto forScriptLine = lexer.l;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                forScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            forScript.erase(forScript.length() - 1, 1);

            root = new node("for", forLine + baseLine);
            auto ast = new AST(forExprScript, forLine + baseLine);
            ast->parse();
            CHECK(ast);
            root->insert(ast->root);
            if (ast->root->value() != "in" && ast->root->value() != "of") {
                ast->parse();
                CHECK(ast);
                root->insert(ast->root);
                ast->parse();
                CHECK(ast);
                root->insert(ast->root);
            }
            root->insert(forScript, forScriptLine + baseLine);

            delete ast;
            break;
        }
        case Lexer::DEF_TOKEN:
        {
            auto defLine = lexer.l;

            GET;
            string funcName(token.s);
            GET;

            enum ArgType {
                PUBLIC, PRIVATE
            };
            struct Arg {
                ArgType type;
                string arg;
                string defaultValue = "";
            };

            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
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
                    if (t.t == Lexer::PUBLIC_TOKEN) {
                        auto name = l.get().s;
                        auto t2 = l.get();
                        if (t2.t == Lexer::SET_TOKEN) {
                            t2 = l.get();
                            while (t2.t != Lexer::PROGRAM_END) {
                                defaultValue += " " + t2.s;
                                t2 = l.get();
                            }
                        }
                        args.push_back(Arg({ PUBLIC, name, defaultValue }));
                    } else if (t.t == Lexer::UNKNOWN_TOKEN) {
                        auto t2 = l.get();
                        if (t2.t == Lexer::SET_TOKEN) {
                            t2 = l.get();
                            while (t2.t != Lexer::PROGRAM_END) {
                                defaultValue += " " + t2.s;
                                t2 = l.get();
                            }
                        }
                        args.push_back(Arg({ PUBLIC, t.s, defaultValue }));
                    } else if (t.t == Lexer::PRIVATE_TOKEN) {
                        auto name = l.get().s;
                        auto t2 = l.get();
                        if (t2.t == Lexer::SET_TOKEN) {
                            t2 = l.get();
                            while (t2.t != Lexer::PROGRAM_END) {
                                defaultValue += " " + t2.s;
                                t2 = l.get();
                            }
                        }
                        args.push_back(Arg({ PRIVATE, name, defaultValue }));
                    } else {
                        root = new node("bad-tree", lexer.l + baseLine);
                        root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
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
            if (t.t == Lexer::PUBLIC_TOKEN) {
                auto name = l.get().s;
                auto t2 = l.get();
                if (t2.t == Lexer::SET_TOKEN) {
                    t2 = l.get();
                    while (t2.t != Lexer::PROGRAM_END) {
                        defaultValue += " " + t2.s;
                        t2 = l.get();
                    }
                }
                args.push_back(Arg({ PUBLIC, name, defaultValue }));
            } else if (t.t == Lexer::UNKNOWN_TOKEN) {
                auto t2 = l.get();
                if (t2.t == Lexer::SET_TOKEN) {
                    t2 = l.get();
                    while (t2.t != Lexer::PROGRAM_END) {
                        defaultValue += " " + t2.s;
                        t2 = l.get();
                    }
                }
                args.push_back(Arg({ PUBLIC, t.s, defaultValue }));
            } else if (t.t == Lexer::PRIVATE_TOKEN) {
                auto name = l.get().s;
                auto t2 = l.get();
                if (t2.t == Lexer::SET_TOKEN) {
                    t2 = l.get();
                    while (t2.t != Lexer::PROGRAM_END) {
                        defaultValue += " " + t2.s;
                        t2 = l.get();
                    }
                }
                args.push_back(Arg({ PRIVATE, name, defaultValue }));
            } else {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }

            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                root = new node("bad-tree", lexer.l + baseLine);
                root->insert("Uncaught SyntaxError: Unexpected token " + token.s, lexer.l + baseLine);
                return;
            }
            string funcScript;
            bcCount = 1;
            auto funcScriptLine = lexer.l;
            while (bcCount > 0) {
                GET;
                if (token.t == Lexer::PROGRAM_END) {
                    root = new node("bad-tree", lexer.l + baseLine);
                    root->insert("Uncaught SyntaxError: Lack of parentheses", lexer.l + baseLine);
                    return;
                }
                funcScript += " " + token.s;
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcCount++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bcCount--;
            }
            funcScript.erase(funcScript.length() - 1, 1);

            auto dl = defLine + baseLine;
            root = new node("def", dl);
            root->insert(funcName, dl);
            root->insert("args", dl);
            for (auto i = 0; i < args.size(); i++) {
                Arg& a = args[i];
                root->get(1)->insert(a.arg, dl);
                if (a.type == PUBLIC) root->get(1)->get(-1)->insert("public", dl);
                else root->get(1)->get(-1)->insert("private", dl);
                auto ast = new AST(a.defaultValue, dl);
                ast->parse();
                CHECK(ast);
                root->get(1)->get(-1)->insert(ast->root);
                delete ast;
            }
            root->insert(funcScript, funcScriptLine + baseLine);
            break;
        }
        case Lexer::USING_TOKEN:
        {
            UL usingLine = lexer.l;
            string expr;
            GET;
            while (token.t != Lexer::END_TOKEN) {
                expr += token.s;
                GET;
            }
            root = new node("using", usingLine + baseLine);
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            root->insert(ast->root);
            delete ast;
            break;
        }
        case Lexer::BREAK_TOKEN:
        {
            UL breakLine = lexer.l;
            string expr;
            GET;
            while (token.t != Lexer::END_TOKEN) {
                expr += token.s;
                GET;
            }
            root = new node("break", breakLine + baseLine);
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
            CHECK(ast);
            if (ast->root->value() != "undefined") root->insert(ast->root);
            else {
                root->insert(new node("1", lexer.l + baseLine));delete ast->root;
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
            }
            auto ast = new AST(expr, lexer.l + baseLine);
            ast->parse();
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
        default:
            root = new node("undefined", lexer.l + baseLine);
            break;
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