#include <cctype>
#include "btype.h"
using std::string;

BerryMath::TYPE BerryMath::type(string data) {
    bool digital = true;
    for (int i = 0 ; i < data.length() ; i++) {
        if (!isdigit(data[i]) && data[i] != '.') {
            digital = false;
            break;
        }
    }
    if (digital) return NUMBER;
    if (
            (data[0] == '\'' && data[data.length() - 1] == '\'') ||
            (data[0] == '\"' && data[data.length() - 1] == '\"')
            ) {
        return STRING;
    }
    if (data[0] == '{' && data[data.length() - 1] == '}') {
        return OBJECT;
    }
    if (data == "undefined") return UNDEFINED;
    return TOKEN_NAME;
}
bool BerryMath::isSymbol(char c) {
    return (
            c == ' ' || c == '!' || c == '"' || c == '\''
            || c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == '(' || c == ')' || c == '&' || c == '|'
            || c == '{' || c == '}' || c == '[' || c == ']' || c == '='
            || c == '<' || c == '>'
    );
}
bool BerryMath::isTrue(string s) {
    auto t = type(s);
    switch (t) {
        case OBJECT:
        case NATIVE_FUNCTION:
        case FUNCTION:
            return true;
        case UNDEFINED:
            return false;
        case STRING:
            if (s == "''" || s == "\"\"") return false;
            return true;
        case NUMBER:
            if (atof(s.c_str()) == 0) return false;
            return true;
        default:
            return false;
    }
}
bool BerryMath::isTokenName(string name) {
    bool res = !(name[0] >= '0' && name[0] <= '9');// rule 1: 不以数字开头
    for (int i = 0; i < name.length(); i++) {// rule 2: 其中没有运算符等符号
        res = res && (!isSymbol(name[i]));
    }
    return res;
}