#include <iostream>
#include "vector"
#include "lex.h"
#include "color.h"
#include "AST.h"
#include "btype.h"
#include "stringpp.h"

void BerryMath::AST::parse() {
    if (root) delete root;
    root = new ASTNode(ROOT, "root");
    lex lexer(code);
    lex::lexToken t;
    lex::lexToken unknown;// 存储目前未知的token
    lex::lexToken tmp;// 存储中间token
    unknown.token = NONE_TOKEN;
    bool expression = false;
//    ASTNode *now;
//    now = root;
    bool first = true;
    int s = lexer.parseIndex;
//    std::cout << "=====" << code << "=====" << std::endl;
    while (true) {
        t = lexer.get();
        if (t.token == IF_TOKEN) {
            int bracketsCount(0);// 首先存储小括号次数
            lex::lexToken op_t;
            bool exitLoop(false);
            string expression("");
            do {
                op_t = lexer.get();
                if (op_t.str == "(") bracketsCount++;
                if (op_t.str == ")") bracketsCount--;
                expression += op_t.str;
                if (op_t.token == END_TOKEN && bracketsCount != 0) {
                    root->str = "bad-tree";
                    exitLoop = true;
                    break;
                }
            } while (bracketsCount != 0);
            std::cout << expression << std::endl;
            if (exitLoop) break;
            string then("");// 存储接下来的语句
            bracketsCount = 0;// 存储大括号次数
            exitLoop = false;
            bool noBrackets = true;
            do {
                if (code[lexer.parseIndex] == '{') {
                    if (bracketsCount > 1) then += code[lexer.parseIndex];
                    bracketsCount++;
                    noBrackets = false;
                } else if (code[lexer.parseIndex] == '}') {
                    if (bracketsCount > 1) then += code[lexer.parseIndex];
                    bracketsCount--;
                    noBrackets = false;
                } else {
                    then += code[lexer.parseIndex];
                }
                lexer.parseIndex++;
            } while ((noBrackets || bracketsCount != 0) && lexer.parseIndex < code.length());
//            if (exitLoop) break;
            AST expressionAST(expression);
            expressionAST.parse();
            root->push(OPERATOR, "if");
            root->at(-1)->push(expressionAST.value()->at(0));
            root->at(-1)->push(VALUE, then);
//            std::cout << "abc" << std::endl;
            return;
        }
        if (t.token == FOR_TOKEN) {
            std::cout << "for token" << std::endl;
        }
        if (t.token == END_TOKEN) {
            if (unknown.token != NONE_TOKEN) {
//                    std::cerr << BOLDRED << "SyntaxError: Unexpected token '" << unknown.str << "'." << RESET << std::endl;
                root->str = "bad-tree";
                break;
            } else {
                break;
            }
        }
        if (first) {
//            std::cout << t.str << ": " << (t.token == UNKNOWN_TOKEN) << std::endl;
            if (!(t.token > SELF_SUB_TOKEN && t.token < INIT_TOKEN)) {// 是表达式
                expression = true;
            }
            first = false;
        }
//        std::cout << t.token << ", " << t.str << std::endl;
        if (expression) {
//            std::cout << "123" << std::endl;
            string left("");
            string right("");
            int base = 1;
            short minPri(15);
            int minOpIndex(-1);
            long tokenLen(0);
            bool callFunction(false);
            string minOp("");
            lex::lexToken op_t;
            op_t.token = INIT_TOKEN;
            lex::lexToken unknow;
            unknow.token = INIT_TOKEN;
            while (op_t.token != END_TOKEN) {
                op_t = lexer.get();
                if (op_t.token > VARIABLE_TOKEN && op_t.token < MINUS_TOKEN) {// 是符号
                    short pri = priority(op_t.str);
                    if (op_t.str == "(") {
                        if (unknow.token != INIT_TOKEN) {// 代表是一个function
                            std::cout << code << std::endl;
                            std::cout << "is function" << std::endl;
                            if (FUNCTION_PRI < minPri) {
//                            std::cout << op_t.str << std::endl;
                                callFunction = true;
                                minOpIndex = lexer.parseIndex;// 括号开始
                                minOp = op_t.str;
                                minPri = pri;
                                tokenLen = 0;
                                int functionBrackets(0);
                                for (int i = minOpIndex - 1; i < code.length(); i++) {
                                    tokenLen++;
                                    if (code[i] == '(') functionBrackets++;
                                    if (code[i] == ')') functionBrackets--;
                                    if (functionBrackets == 0) break;
                                }
                            }
                            // function 调用的级别
                            unknow.token = INIT_TOKEN;
                        }
                        base *= pri;
                    } else if (op_t.str == ")") {
                        base /= pri;
                    }
                    pri *= base;
                    if (!isNumber(op_t.str) && pri < minPri && op_t.str != "(" && op_t.str != ")") {
//                            std::cout << op_t.str << std::endl;
                        minOpIndex = lexer.parseIndex;
                        minOp = op_t.str;
                        minPri = pri;
                        tokenLen = op_t.str.length();
                    }
                } else {
//                    std::cout << op_t.str << std::endl;
                    if (op_t.token == UNKNOWN_TOKEN) {
//                        std::cout << op_t.str << ";" << std::endl;
                        if (unknow.token == INIT_TOKEN) {
                            unknow = op_t;
                        } else {
                            root->str = "bad-tree";
                            break;
                        }
                    }
                }
            }
            if (code == " number(\"123\");") {
                std::cout << minOpIndex << ", " << minOp << ", " << minPri << ", " << tokenLen << "; " << std::endl;
                std::cout << "m" << std::endl;
            }
//            std::cout << minOpIndex << std::endl;
            if (minOpIndex == -1) {// 说明没有符号
                lexer.parseIndex = s;
                auto n = lexer.get();
//                std::cout << n.str << std::endl;
//                int countXX(0);
                while (n.str == "(" || n.str == ")") {
                    n = lexer.get();
//                    countXX++;
                }
                n.str = trim(n.str);
                if (priority(n.str) == 13) {// ++, --, !, ~
                    root->push(OPERATOR, n.str);
//                    n = lexer.get();
                    int bracketsCount(0);
                    string then("");
                    for (int i = lexer.parseIndex; i < code.length(); i++) {
                        if (code[i] == '(') bracketsCount++;
                        if (code[i] == ')') bracketsCount--;
                        then += code[i];
                    }
                    if (bracketsCount < 0) {
                        for (int i = bracketsCount; i < 0; i++) {
                            then = "(" + then;
                        }
                    } else {
                        for (int i = 0; i < bracketsCount; i++) {
                            then += ")";
                        }
                    }
                    auto ast = new AST(then);
                    ast->parse();
                    root->at(-1)->push(ast->root->at(-1));
//                    std::cout << "tmp" << std::endl;
                } else {
                    root->push(VALUE, n.str);
//                    root->at(-1)->push(ast->root->at(-1));
                }
                break;
            }
//            std::cout << minOpIndex << ", " << minPri << ", " << minOp << std::endl;
            minOpIndex--;
//            std::cout << (minOp == "=") << std::endl;

            if (code == " number(\"123\");") {
                std::cout << "MinOp: " << minOpIndex << ", " << minOp << std::endl;
            }

            if (callFunction) {
                root->push(OPERATOR, "call");
                root->at(-2)->push(OPERATOR, minOp);

                string in("");
                for (int i = minOpIndex; i < minOpIndex + tokenLen; i++) {
                    in += code[i];
                }
                std::cout << in << std::endl;
//                auto expressions = spilt();
            } else {
                root->push(OPERATOR, minOp);

                int bracketsCount(0);
                for (int i = s; i < minOpIndex - tokenLen + 1; i++) {
                    if (code[i] == '(') bracketsCount++;
                    if (code[i] == ')') bracketsCount--;
                    left += code[i];
                }
//            std::cout << "bracketsCountLeft: " << bracketsCount << std::endl;
                if (bracketsCount > 0) {
                    for (int i = 0; i < bracketsCount; i++) {
                        left += ")";
                    }
                } else {
                    for (int i = bracketsCount; i < 0; i++) {
                        left = "(" + left;
                    }
                }
//            std::cout << left << std::endl;
                bracketsCount = 0;
                for (int i = minOpIndex + 1; i < code.length(); i++) {
                    if (code[i] == '(') bracketsCount++;
                    if (code[i] == ')') bracketsCount--;
                    right += code[i];
                }
//            std::cout << "bracketsCountRight: " << bracketsCount << std::endl;
                if (bracketsCount > 0) {
                    for (int i = 0; i < bracketsCount; i++) {
                        right += ")";
                    }
                } else {
                    for (int i = bracketsCount; i < 0; i++) {
                        right = "(" + right;
                    }
                }
//            std::cout << right << std::endl;
                bracketsCount = 0;
//            std::cout << left << minOp << right << std::endl;
                auto leftAST = new AST(left);
                auto rightAST = new AST(right);
                leftAST->parse();
                rightAST->parse();
                if (
                        leftAST->value()->value() == "bad-tree"
                        || rightAST->value()->value() == "bad-tree"
                        ) {
                    root->str = "bad-tree";
                    break;
                }
//            std::cout << code << std::endl;
//            std::cout << left << std::endl;
//            std::cout << right << std::endl;
                root->at(-1)->push(leftAST->root->at(-1));
                root->at(-1)->push(rightAST->root->at(-1));
                if (code == "\ntest += number(\"123\");") {
                    std::cout << "last" << std::endl;
                }
                continue;
            }
        }
    }
//    root->each([](ASTNode* n) {
//        std::cout << BOLDMAGENTA << n->str << ", " << n->t << RESET << std::endl;
//    });
//    std::cout << BOLDCYAN << "[SystemInfo] Build AST finish." << RESET << std::endl;
}

short BerryMath::AST::priority(string op) {
//    std::cout << "op: " << op << std::endl;
    if (op == "(" || op == "[" || op == ".") {
        return 14;
    }
    if (op == "++" || op == "--" || op == "!" || op == "~") {
        return 13;
    }
    if (op == "*" || op == "/" || op == "%") {
        return 12;
    }
    if (op == "+" || op == "-") {
        return 11;
    }
    if (op == "<<" || op == ">>") {
        return 10;
    }
    if (op == ">" || op == ">=" || op == "<" || op == "<=") {
        return 9;
    }
    if (op == "==" || op == "!=") {
        return 8;
    }
    if (op == "&") {
        return 7;
    }
    if (op == "^") {
        return 6;
    }
    if (op == "|") {
        return 5;
    }
    if (op == "&&") {
        return 4;
    }
    if (op == "||") {
        return 3;
    }
    if (
            op == "="
            || op == "*=" || op == "/=" || op == "%="
            || op == "+=" || op == "-="
            || op == ">>=" || op == "<<="
            || op == "^=" || op == "&=" || op == "|="
            ) {
        return 2;
    }
    if (op == ",") {
        return 1;
    }
    return 15;
}

bool BerryMath::isNumber(string n) {
    bool dot = false;
    for (int i = 0; i < n.length(); i++) {
        if (!(n[i] >= '0' && n[i] <= '9')) {
            if (n[i] == '.')
                if (dot) return false;
                else dot = true;
            else
                return false;
        }
    }
    return true;
}