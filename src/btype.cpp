#include <cctype>
#include "btype.h"

BerryMath::TYPE BerryMath::type(std::string data) {
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
bool BerryMath::isTrue(std::string s) {
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
            if (s == "0") return false;
            return true;
        default:
            return false;
    }
}