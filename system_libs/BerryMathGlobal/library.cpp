#include "library.h"

#include <iostream>
#include <string>
#include <vector>
#include <BerryMath.h>
#include <regex>

BerryMath::value* Number(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    auto v = arguments[0];
    if (v->typeOf() == BerryMath::NUMBER) {
        return new BerryMath::value(BerryMath::NUMBER, v->valueOf());
    }
    if (v->typeOf() == BerryMath::STRING) {
        auto str = v->valueOf();
        str.erase(str.begin());
        str.erase(str.end() - 1);
        return new BerryMath::value(BerryMath::NUMBER, std::to_string(atof(str.c_str())));
    }
    return new BerryMath::value(BerryMath::NUMBER, "0");
}
BerryMath::value* String(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    auto v = arguments[0];
    if (v->typeOf() == BerryMath::NUMBER) {
        return new BerryMath::value(BerryMath::STRING, "\"" + v->valueOf() + "\"");
    }
    if (v->typeOf() == BerryMath::STRING) {
        return new BerryMath::value(BerryMath::STRING, v->valueOf());
    }
    if (v->typeOf() == BerryMath::FUNCTION || v->typeOf() == BerryMath::NATIVE_FUNCTION || v->typeOf() == BerryMath::OBJECT) {
        return new BerryMath::value(BerryMath::STRING, "\"\"");
    }
    return new BerryMath::value(BerryMath::STRING, "\"undefined\"");
}

BerryMath::value* print(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    long sz = arguments.size();
    std::regex patternNewLine("\\\\n", std::regex::icase);
    std::regex patternTable("\\\\t", std::regex::icase);
    for (int i = 0; i < sz; i++) {
        auto type = arguments[i]->typeOf();
        switch (type) {
            case BerryMath::NUMBER:
            case BerryMath::UNDEFINED:
                std::cout << arguments[i]->valueOf() << " ";
                break;
            case BerryMath::NATIVE_FUNCTION:
            case BerryMath::FUNCTION:
                std::cout << "<Function at " << std::to_string((long) arguments[i]) << "> ";
                break;
            case BerryMath::OBJECT:
                std::cout << "<Object at " << std::to_string((long) arguments[i]) << "> ";
                break;
            case BerryMath::STRING:
                std::string n = arguments[i]->valueOf();
                n.erase(n.begin());
                n.erase(n.end() - 1);
                n = std::regex_replace(n, patternNewLine, "\n");
                n = std::regex_replace(n, patternTable, "\t");
                std::cout << n << " ";
                break;
        }
    }
    return new BerryMath::value(BerryMath::UNDEFINED, "undefined");
}
BerryMath::value* println(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    print(arguments, argumentsHash);
    std::cout << std::endl;
    return new BerryMath::value(BerryMath::UNDEFINED, "undefined");
}