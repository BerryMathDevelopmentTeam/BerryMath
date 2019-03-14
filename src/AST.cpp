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
    bool expression = false;
    ASTNode *now;
    now = root;
    bool first = true;
    int s = lexer.parseIndex;
    std::cout << code << std::endl;
    while (true) {
        t = lexer.get();
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
            short minPri(15);
            int minOpIndex(-1);
            string minOp("");
            lex::lexToken op_t;
            op_t.token = INIT_TOKEN;
            while (op_t.token != END_TOKEN) {
                op_t = lexer.get();
                if (op_t.token > VARIABLE_TOKEN && op_t.token < MINUS_TOKEN) {// 是符号
                    short pri = priority(op_t.str);
                    if (pri < minPri) {
                        minOpIndex = lexer.parseIndex;
                        minOp = op_t.str;
                        minPri = pri;
                    }
                }
            }
            std::cout << minOpIndex << " of '" << code << "'" << std::endl;
            if (minOpIndex == -1) {// 说明没有符号
                lexer.parseIndex = s;
                auto n = lexer.get();
                root->push(VALUE, n.str);
                break;
            }
//            std::cout << minOpIndex << ", " << minPri << ", " << minOp << std::endl;
            minOpIndex--;
//            std::cout << (minOp == "=") << std::endl;
            root->push(OPERATOR, minOp);
            for (int i = s; i < minOpIndex; i++) {
                left += code[i];
            }
            for (int i = minOpIndex + 1; i < code.length(); i++) {
                right += code[i];
            }
//            std::cout << left << minOp << right << std::endl;
            auto leftAST = new AST(left);
            auto rightAST = new AST(right);
            leftAST->parse();
            rightAST->parse();
            std::cout << code << std::endl;
            std::cout << left << std::endl;
            std::cout << right << std::endl;
            root->at(-1)->push(leftAST->root->at(-1));
            root->at(-1)->push(rightAST->root->at(-1));
//            std::cout << "tmp: " << code << std::endl;
        } else {
        }
    }
//    root->each([](ASTNode* n) {
//        std::cout << BOLDMAGENTA << n->str << ", " << n->t << RESET << std::endl;
//    });
    std::cout << BOLDCYAN << "[SystemInfo] Build AST finish." << RESET << std::endl;
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