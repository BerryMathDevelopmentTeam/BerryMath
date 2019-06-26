#ifndef BERRYMATH_LEX_H
#define BERRYMATH_LEX_H

#include <string>
using std::string;
#define ULL unsigned long long
#define UL unsigned long
#define U unsigned

namespace BM {
    class Lexer {
    public:
        enum TOKENS {
            NO_STATUS = 0,// 初始化状态
            UNKNOWN_TOKEN,// 未知token
            // key words
            LET_TOKEN, DEF_TOKEN,
            IF_TOKEN, ELIF_TOKEN, ELSE_TOKEN, SWITCH_TOKEN, CASE_TOKEN, DEFAULT_TOKEN,
            FOR_TOKEN, WHILE_TOKEN, DO_TOKEN, CONTINUE_TOKEN, BREAK_TOKEN,
            RETURN_TOKEN,
            ENUM_TOKEN,
            USING_TOKEN,
            CLASS_TOKEN, PUBLIC_TOKEN, PRIVATE_TOKEN, NEW_TOKEN,
            NULL_TOKEN, UNDEFINED_TOKEN,
            NUMBER_TOKEN, STRING_TOKEN,
            // 注释token
            NOTE_TOKEN,
            // 符号token
            UNKNOWN_OP_TOKEN,
            SET_TOKEN, COLON_TOKEN,
            EQUAL_TOKEN, NOT_EQUAL_TOKEN, LEQUAL_TOKEN, MEQUAL_TOKEN,
            LESS_TOKEN, MORE_TOKEN,
            BRACKETS_LEFT_TOKEN, BRACKETS_RIGHT_TOKEN,
            MIDDLE_BRACKETS_LEFT_TOKEN, MIDDLE_BRACKETS_RIGHT_TOKEN,
            BIG_BRACKETS_LEFT_TOKEN, BIG_BRACKETS_RIGHT_TOKEN,
            DOT_TOKEN, COMMA_TOKEN,
            MAND_TOKEN, MOR_TOKEN, MXOR_TOKEN, MNOT_TOKEN,
            LAND_TOKEN, LOR_TOKEN, LNOT_TOKEN,
            ADD_TOKEN, SUB_TOKEN, MUL_TOKEN, DIV_TOKEN, MOD_TOKEN, POWER_TOKEN,
            SLEFT_TOKEN, SRIGHT_TOKEN,
            ADD_TO_TOKEN, SUB_TO_TOKEN, MUL_TO_TOKEN, DIV_TO_TOKEN, MOD_TO_TOKEN, POWER_TO_TOKEN,
            SLEFT_TO_TOKEN, SRIGHT_TO_TOKEN,
            MAND_TO_TOKEN, MOR_TO_TOKEN, MXOR_TO_TOKEN,
            DADD_TOKEN, DSUB_TOKEN,// double add & double sub
            IN_TOKEN, OF_TOKEN,
            END_TOKEN,// 语句结束
            PROGRAM_END
        };
        struct Token {
            TOKENS t;
            string s;
        };
        Lexer() : script(""), i(0), l(1) { }
        Lexer(const string& s) : script(s), i(0), l(1) { }
        void open(const string& s) {
            script = s;
            i = 0;
            l = 1;
        }
        Token get();
        Token token() { return t; }
        UL index() { return i; }
        UL line() { return l; }
    private:
        Token t;
        string script;
        UL i;
        UL l;
        bool updateLine = false;
        friend class AST;
#define IS_SPACE(c) (c == '\t' || c == ' ' || c == '\n')
#define IS_OP(c) \
    (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') && c != '_' && c != '$')
    };

#define IS_NUM(c) (c >= '0' && c <= '9')
}

#endif //BERRYMATH_LEX_H
