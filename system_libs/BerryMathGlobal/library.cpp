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
BerryMath::value* Length(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    auto v = arguments[0];
    if (v->typeOf() == BerryMath::STRING) {
        return new BerryMath::value(BerryMath::NUMBER, std::to_string(v->valueOf().length()));
    }
    return new BerryMath::value(BerryMath::NUMBER, "0");
}
BerryMath::value* Type(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    auto v = arguments[0];
    auto t = BerryMath::type(v->valueOf());
    switch (t) {
        case BerryMath::NUMBER:
            return new BerryMath::value(BerryMath::STRING, "\"number\"");
        case BerryMath::STRING:
            return new BerryMath::value(BerryMath::STRING, "\"string\"");
        case BerryMath::OBJECT:
            return new BerryMath::value(BerryMath::STRING, "\"object\"");
        case BerryMath::FUNCTION:
        case BerryMath::NATIVE_FUNCTION:
            return new BerryMath::value(BerryMath::STRING, "\"function\"");
    }
    return new BerryMath::value(BerryMath::STRING, "\"undefined\"");
}

BerryMath::value* print(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    long sz = arguments.size();
//    std::regex patternNewLine("\\\\n", std::regex::icase);
//    std::regex patternTable("\\\\t", std::regex::icase);
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
                bool trans = false;
                for (auto i = 0; i < n.length(); i++) {
                    if (n[i] == '\\') {
                        trans = !trans;
                        if (!trans) {
                            std::cout << "\\";
                        }
                    } else {
                        if (trans) {
                            trans = false;
                            switch (n[i]) {
                                case 'a':
                                    std::cout << "\a";
                                    break;
                                case 'b':
                                    std::cout << "\b";
                                    break;
                                case 'f':
                                    std::cout << "\f";
                                    break;
                                case 'n':
                                    std::cout << "\n";
                                    break;
                                case 'r':
                                    std::cout << "\r";
                                    break;
                                case 't':
                                    std::cout << "\t";
                                    break;
                                case 'v':
                                    std::cout << "\v";
                                    break;
                                case '\'':
                                    std::cout << "\'";
                                    break;
                                case '\"':
                                    std::cout << "\"";
                                    break;
                                case '\?':
                                    std::cout << "\?";
                                    break;
                                case '0':
                                    if (i + 2 < n.length() && n[i + 1] == '3' && n[i + 2] == '3') {
                                        string t("");
                                        for (i += 3; i < n.length() && n[i] != 'm'; i++) t += n[i];
                                        t += n[i];
//                                        std::cout << t << std::endl;
                                        std::cout << ("\033" + t);
                                    } else {
                                        std::cout << " ";
                                    }
                                    break;
                                default:
                                    std::cout << " ";
                                    break;
                            }
                        } else {
                            std::cout << n[i];
                        }
                    }
                }
                std::cout << " ";
//                n = std::regex_replace(n, patternNewLine, "\n");
//                n = std::regex_replace(n, patternTable, "\t");
//                std::cout << n << " ";
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

BerryMath::value* input(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    print(arguments, argumentsHash);
    string v;
    std::cin >> v;
    auto t = BerryMath::type(v);
    if (t == BerryMath::TOKEN_NAME) {
        t = BerryMath::STRING;
        v = "\"" + v + "\"";
    }
    return new BerryMath::value(t, v);
}
BerryMath::value* Exit(std::vector<BerryMath::value*> arguments, std::map<std::string, BerryMath::value*> argumentsHash) {
    string c("0");
    if (arguments.size() >= 1) c = arguments[0]->valueOf();
    int code = atoi(c.c_str());
    exit(code);
//    return new BerryMath::value(BerryMath::UNDEFINED, "undefined");
}