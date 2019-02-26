#include <iostream>
#include "lex.h"
#include "color.h"
#include "AST.h"

void BerryMath::AST::parse() {
    if (root) delete root;
    root = new ASTNode(ROOT, "root");
    lex lexer(code);
    lex::lexToken t;
    lex::lexToken unknown;// 存储目前未知的token
    lex::lexToken tmp;// 存储中间token
    unknown.token = NONE_TOKEN;
    ASTNode *now;
    ASTNode *origin = nullptr;// 存储某些语句的原点(出发点), 比如if语句
    lex::lexToken originToken;// 存储某些语句的原点(出发点), 比如if语句
    now = root;
    bool inExpression = false;
    while (true) {
        t = lexer.get();
//        std::cout << t.token << ", " << t.str << std::endl;
        if (t.token == UNKNOWN_TOKEN && inExpression) {
            t.token = VARIABLE_TOKEN;
            tmp = t;
            t = lexer.get();
            now->push(OPERATOR, "expression");
            if (t.token == END_TOKEN) {
                now->at(-1)->push(VALUE, tmp.str);
                break;
            }
            if (t.token >= BIGGER_TOKEN && t.token <= SELF_SUB_TOKEN) {// 是符号操作
                switch (t.token) {
                    case ADD_TOKEN:
                        now->at(-1)->push(OPERATOR, "add");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case SUB_TOKEN:
                        now->at(-1)->push(OPERATOR, "sub");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case MUL_TOKEN:
                        now->at(-1)->push(OPERATOR, "mul");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case DIV_TOKEN:
                        now->at(-1)->push(OPERATOR, "div");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case MOD_TOKEN:
                        now->at(-1)->push(OPERATOR, "mod");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case SELF_ADD_TOKEN:
                        now->at(-1)->push(OPERATOR, "self-add");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        break;
                    case SELF_SUB_TOKEN:
                        now->at(-1)->push(OPERATOR, "self-sub");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        break;
                }
            }
        }
        if (t.token == END_TOKEN) {
            if (unknown.token != NONE_TOKEN) {
                std::cerr << BOLDRED << "SyntaxError: Unexpected token '" << unknown.str << "'." << RESET << std::endl;
                root->str = "bad-tree";
                break;
            } else {
                break;
            }
        }
        if (t.token == UNKNOWN_TOKEN) {
            if (unknown.token == NONE_TOKEN) {
//                std::cout << t.str << std::endl;
                unknown = t;
                continue;
            } else {
                std::cerr << BOLDRED << "SyntaxError: Unexpected token '" << unknown.str << "' near '" << t.str << "'."
                          << RESET << std::endl;
                root->str = "bad-tree";
            }
            break;
        }
        if (t.token == IF_TOKEN) {
//            std::cout << "123" << std::endl;
            if (unknown.token != NONE_TOKEN) {
                std::cerr << BOLDRED << "SyntaxError: Unexpected token '" << unknown.str << "'." << RESET << std::endl;
                continue;
            }
            now->push(OPERATOR, "if");
            now->at(-1)->push(OPERATOR, "expression");
            auto next = lexer.get();
//            std::cout << next.str << std::endl;
            if (next.token != BRACKETS_LEFT_TOKEN) {
                std::cerr << BOLDRED << "SyntaxError: Unexpected token near 'if'." << RESET << std::endl;
            }
            origin = now->at(-1);
            originToken = t;
            now = now->at(-1)->at(-1);
            unknown.token = NONE_TOKEN;
            inExpression = true;
        }
//        std::cout << t.str << std::endl;
        if (t.token == BRACKETS_RIGHT_TOKEN) {// 右括号，这说明是if语句等的结束
            std::cout << "123" << std::endl;
            switch (originToken.token) {
                case IF_TOKEN:
                    now = origin;
                    std::cout << "if" << std::endl;
                    break;
                default:
                    std::cerr << BOLDRED << "SyntaxError: Unexpected token ')'." << RESET << std::endl;
                    break;
            }
            inExpression = false;
        }
        if (t.token == SET_TOKEN) {
            if (unknown.token == NONE_TOKEN) {
//                std::cout << t.str << std::endl;
                std::cerr << BOLDRED << "SyntaxError: Unexpected token '='." << RESET << std::endl;
                continue;
            }
            now->push(OPERATOR, "set");
            now->at(-1)->push(VALUE, unknown.str);
            now->at(-1)->push(OPERATOR, "expression");
            now = now->at(-1)->at(-1);
            unknown.token = NONE_TOKEN;
            inExpression = true;
        }
        if (t.token == NUMBER_TOKEN) {
            tmp = t;
            t = lexer.get();
            now->push(OPERATOR, "expression");
            if (t.token == END_TOKEN) {
                now->at(-1)->push(VALUE, tmp.str);
                break;
            }
            if (t.token >= BIGGER_TOKEN && t.token <= SELF_SUB_TOKEN) {// 是符号操作
                switch (t.token) {
                    case ADD_TOKEN:
                        now->at(-1)->push(OPERATOR, "add");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case SUB_TOKEN:
                        now->at(-1)->push(OPERATOR, "sub");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case MUL_TOKEN:
                        now->at(-1)->push(OPERATOR, "mul");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case DIV_TOKEN:
                        now->at(-1)->push(OPERATOR, "div");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case MOD_TOKEN:
                        now->at(-1)->push(OPERATOR, "mod");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                    case EQUAL_TOKEN:
//                        std::cout << "n" << std::endl;
                        now->at(-1)->push(OPERATOR, "equal");
                        now->at(-1)->at(-1)->push(VALUE, tmp.str);
                        now->at(-1)->at(-1)->push(OPERATOR, "expression");
                        now = now->at(-1)->at(-1)->at(-1);
                        break;
                }
            }
        }
    }
//    root->each([](ASTNode* n) {
//        std::cout << BOLDMAGENTA << n->str << ", " << n->t << RESET << std::endl;
//    });
//    std::cout << BOLDCYAN << "[SystemInfo] Build AST finish." << RESET << std::endl;
}