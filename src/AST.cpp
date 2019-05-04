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
        if (first) {
//            std::cout << t.str << ": " << (t.token == UNKNOWN_TOKEN) << std::endl;
            if (!(t.token > SELF_SUB_TOKEN && t.token < INIT_TOKEN) || t.token == UNKNOWN_TOKEN) {// 是表达式
                expression = true;
            }
            int nowIndex = lexer.parseIndex;
            auto test = lexer.get();
            if (t.token == UNKNOWN_TOKEN && test.token == BRACKETS_LEFT_TOKEN) {
//                expression = true;
                lexer.parseIndex = 0;
            } else {
                lexer.parseIndex = nowIndex;
            }
        }
        first = false;
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
//            std::cout << expression << std::endl;
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
            root->push(OPERATOR, "if");
            root->at(-1)->push(VALUE, expression);
            root->at(-1)->push(VALUE, then);
            auto rawIndex = lexer.parseIndex;
            lex::lexToken thenT;
            do {
                rawIndex = lexer.parseIndex;
                thenT = lexer.get();
                if (thenT.token == ELIF_TOKEN) {
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
//            std::cout << expression << std::endl;
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
                    root->at(-1)->push(OPERATOR, "elif");
                    root->at(-1)->at(-1)->push(VALUE, expression);
                    root->at(-1)->at(-1)->push(VALUE, then);
                } else if (thenT.token == ELSE_TOKEN) {
                    int bracketsCount(0);// 首先存储小括号次数
                    lex::lexToken op_t;
                    string then("");// 存储接下来的语句
                    bracketsCount = 0;// 存储大括号次数
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
                    root->at(-1)->push(OPERATOR, "else");
                    root->at(-1)->at(-1)->push(VALUE, then);
                    break;
                } else {
                    lexer.parseIndex = rawIndex;
                    break;
                }
            } while (lexer.parseIndex < code.length());
//            std::cout << "if token" << std::endl;
            break;
        }

        if (t.token == WHILE_TOKEN) {
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
//            std::cout << expression << std::endl;
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
//            AST* expressionAST = new AST(expression);
//            expressionAST->parse();
            root->push(OPERATOR, "while");
//            root->at(-1)->push(expressionAST->value()->at(0));
            root->at(-1)->push(VALUE, expression);
            root->at(-1)->push(VALUE, then);
//            std::cout << "while token" << std::endl;
            break;
        }
        if (t.str == "for") {
            // for循环格式:
            // for from s to e with v {
            //      s, e为一个表达式, v为循环变量
            // }


            // get from token
            t = lexer.get();
            if (t.token != FROM_TOKEN) {
                root->str = "bad-tree";
                break;
            }


            // get from的值
            string exprFrom("");
            lex::lexToken op_t = lexer.get();
            while (op_t.token != TO_TOKEN && op_t.token != END_TOKEN) {
                exprFrom += op_t.str;
                op_t = lexer.get();
            }
            if (op_t.token != TO_TOKEN) {
                root->str = "bad-tree";
                break;
            }

            // get to的值
            string exprTo("");
            op_t = lexer.get();
            while (op_t.token != WITH_TOKEN && op_t.token != END_TOKEN) {
                exprTo += op_t.str;
                op_t = lexer.get();
            }
            if (op_t.token != WITH_TOKEN) {
                root->str = "bad-tree";
                break;
            }

            // 获取循环变量名
            t = lexer.get();
            string withToken = t.str;

            // 获取循环变量到底是for循环内部局部变量还是全局变量还是for循环所在作用域的变量
            auto tmpParseIndex = lexer.parseIndex;
            string loopFlag("");
            t = lexer.get();
            loopFlag = t.str;
            if (loopFlag != "local" && loopFlag != "global" && loopFlag != "out") {
                loopFlag = "local";// 这说明省略了循环变量的类型，就默认为local
                lexer.parseIndex = tmpParseIndex;
            }

            // 获取for语句中的代码
            t = lexer.get();
            if (t.str != "{") {
                root->str = "bad-tree";
                break;
            }
            string then("{");
            bool exitLoop(false);
            int bracketCount(1);
            int i(lexer.parseIndex);
            for (; i < code.length(); i++) {
                if (code[i] == '{') bracketCount++;
                if (code[i] == '}') bracketCount--;
                then += code[i];
                if (bracketCount == 0) break;
            }

            root->push(OPERATOR, "for");
            root->at(-1)->push(OPERATOR, "range");
            root->at(-1)->at(0)->push(VALUE, exprFrom);
            root->at(-1)->at(0)->push(VALUE, exprTo);
            root->at(-1)->push(OPERATOR, "with");
            root->at(-1)->at(1)->push(VALUE, withToken);
            root->at(-1)->at(1)->push(VALUE, loopFlag);
            root->at(-1)->push(VALUE, then);
        }
        if (t.token == BREAK_TOKEN) {
            root->push(OPERATOR, "break");
        }
        if (t.token == CONTINUE_TOKEN) {
            root->push(OPERATOR, "continue");
        }
        if (t.token == END_TOKEN) {
            if (unknown.token != NONE_TOKEN && unknown.token != INIT_TOKEN) {
//                    std::cerr << BOLDRED << "SyntaxError: Unexpected token '" << unknown.str << "'." << RESET << std::endl;
                root->str = "bad-tree";
                break;
            } else {
                break;
            }
        }
//        std::cout << t.token << ", " << t.str << std::endl;
        if (expression) {
//            std::cout << "123" << std::endl;
            string left("");
            string right("");
            int base = 1;
            int minPri(15);
            int minOpIndex(-1);
            long tokenLen(0);
            bool callFunction(false);
            bool array(false);
            string minOp("");
            lex::lexToken op_t;
            op_t.token = INIT_TOKEN;
//            lex::lexToken unknow;
//            unknow.token = INIT_TOKEN;
//            lexer.parseIndex = 0;
            if (t.token == MIDDLE_BRACKETS_LEFT_TOKEN) {
                lexer.parseIndex = 0;
//                std::cout << lexer.parseIndex << std::endl;
            }
//            lexer.parseIndex = 0;
            while (op_t.token != END_TOKEN) {
                op_t = lexer.get();
                if (op_t.token > VARIABLE_TOKEN && op_t.token < MINUS_TOKEN) {// 是符号
                    int pri = priority(op_t.str);
                    if (op_t.str == "(") {
//                        std::cout << unknown.str << std::endl;
                        if (unknown.token != INIT_TOKEN && unknown.token != NONE_TOKEN) {// 代表是一个function
                            if (FUNCTION_PRI < minPri) {
//                            std::cout << op_t.str << std::endl;
                                callFunction = true;
                                minOpIndex = lexer.parseIndex;// 括号开始
                                minOp = unknown.str;
                                minPri = pri;
                                tokenLen = 0;
//                                std::cout << minOp << std::endl;
                                int functionBrackets(0);
                                for (int i = minOpIndex - 1; i < code.length(); i++) {
                                    tokenLen++;
                                    if (code[i] == '(') functionBrackets++;
                                    if (code[i] == ')') functionBrackets--;
                                    if (functionBrackets == 0) break;
                                }
                            }
                            // function 调用的级别
                            unknown.token = INIT_TOKEN;
                        }
                        base *= pri;
                    } else if (op_t.str == ")") {
//                        base /= pri;
                        base /= priority("]");
                    }
                    if (op_t.str == "[") {
                        if (unknown.token != UNKNOWN_TOKEN && (base * ARRAY_VALUE_PRI) <= minPri && !array) {
                            minOpIndex = lexer.parseIndex;
                            minPri = 15;
                            callFunction = false;
                            array = true;
                        }
                        base *= pri;
                    } else if (op_t.str == "]") {
                        base /= priority("[");
                    }

                    pri *= base;
                    if (!isNumber(op_t.str) && op_t.str != "(" && op_t.str != ")" && pri < minPri && op_t.str != "[" && op_t.str != "]") {
                        minOpIndex = lexer.parseIndex;
                        minOp = op_t.str;
                        minPri = pri;
                        tokenLen = op_t.str.length();
                        callFunction = false;
                        array = false;
                    }
                    if (op_t.str == "[" && !array && pri < minPri) {
                        minOpIndex = lexer.parseIndex;
                        minOp = op_t.str;
                        minPri = pri;
                        tokenLen = op_t.str.length();
                        callFunction = false;
                        array = false;
                    }
                }
                if (op_t.token == UNKNOWN_TOKEN) {
                    if (unknown.token == INIT_TOKEN || unknown.token == NONE_TOKEN) {
                        unknown = op_t;
                    } else {
//                        root->str = "bad-tree";
//                        break;
                    }
                }
            }
            if (minOpIndex == -1 && !array) {// 说明没有符号并且不是数组
                lexer.parseIndex = s;
                auto n = lexer.get();
                while (n.str == "(" || n.str == ")") {
                    n = lexer.get();
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
                } else {
                    root->push(VALUE, n.str);
//                    root->at(-1)->push(ast->root->at(-1));
                }
                break;
            }
            minOpIndex--;

            if (callFunction) {
                root->push(OPERATOR, "call");
                root->at(-1)->push(VALUE, minOp);

                string in("");
                for (int i = minOpIndex; i < minOpIndex + tokenLen; i++) {
                    in += code[i];
                }
                std::vector<string> arguments;
                string expressionOne("");
                int bCount(0);
                for (int i = 0; i < in.length(); i++) {
                    if (in[i] == '(') bCount++;
                    if (in[i] == ')') bCount--;
                    if (in[i] == ',' && bCount == 1) {// 是传参中的其中一个参数
                        expressionOne += ")";
                        arguments.push_back(expressionOne);
                        expressionOne = "";
                    } else {
                        expressionOne += in[i];
                    }
                    if (bCount == 0) {// 函数调用语句结束
                        if (arguments.size() > 0)// 如果是多个参数调用
                            expressionOne = "(" + expressionOne;// 因为给定函数调用语句: func1(a1, a2, a3), 最后一个参数现在就是'a3)', 如果要符合语法, 就要在其前面加(, 但是如果是单个参数就不用, 因为给定函数调用语句: func2(a), 现在参数就是'(a)', 加上(反而就不符合语法了
                        arguments.push_back(expressionOne);
                        expressionOne = "";
                        break;
                    }
                }
                for (int i = 0; i < arguments.size(); i++) {// 遍历所有实参
                    auto ast = new AST(arguments[i]);
                    ast->parse();
                    root->at(-1)->push(ast->value()->at(-1));
                }
                unknown.token = INIT_TOKEN;
                unknown.str = "";
            } else if (array) {
//                std::cout << minOp << std::endl;
                root->push(OPERATOR, "array");

                int bracketsCount(1);
                string arr("");
                vector<string> elesStr;
                for (int i = minOpIndex + 1; i < code.length(); i++) {
                    if (code[i] == '[') bracketsCount++;
                    if (code[i] == ']') {
                        bracketsCount--;
                        if (bracketsCount == 0) {
                            elesStr.push_back(arr);
                            break;
                        }
                    }
                    if (code[i] == ',' && bracketsCount <= 1) {
                        elesStr.push_back(arr);
                        arr = "";
                    } else {
                        arr += code[i];
                    }
//                    std::cout << code[i];
                }
                for (int i = 0; i < elesStr.size(); i++) {
//                    root->at(-1)->push(VALUE, elesStr[i]);
                    auto ast = new AST(elesStr[i]);
                    ast->parse();
                    root->at(-1)->push(ast->value()->at(0));
                }
//                std::cout << "Array";
//                std::cout << std::endl;
                unknown.token = INIT_TOKEN;
                unknown.str = "";
            } else {
                root->push(OPERATOR, minOp);

                int bracketsCount(0);
                for (int i = s; i < minOpIndex - tokenLen + 1; i++) {
                    if (code[i] == '(') bracketsCount++;
                    if (code[i] == ')') bracketsCount--;
                    left += code[i];
                }
                if (bracketsCount > 0) {
                    for (int i = 0; i < bracketsCount; i++) {
                        left += ")";
                    }
                } else {
                    for (int i = bracketsCount; i < 0; i++) {
                        left = "(" + left;
                    }
                }
                bracketsCount = 0;
                for (int i = minOpIndex + 1; i < code.length(); i++) {
                    if (code[i] == '(') bracketsCount++;
                    if (code[i] == ')') bracketsCount--;
                    right += code[i];
                }
                if (bracketsCount > 0) {
                    for (int i = 0; i < bracketsCount; i++) {
                        right += ")";
                    }
                } else {
                    for (int i = bracketsCount; i < 0; i++) {
                        right = "(" + right;
                    }
                }
                bracketsCount = 0;
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
                root->at(-1)->push(leftAST->root->at(-1));
                root->at(-1)->push(rightAST->root->at(-1));
                unknown.token = INIT_TOKEN;
                unknown.str = "";
                continue;
            }
        }
    }

//    std::cout << "Built AST." << std::endl;
    //    root->each([](ASTNode* n) {
//        std::cout << BOLDMAGENTA << n->str << ", " << n->t << RESET << std::endl;
//    });
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