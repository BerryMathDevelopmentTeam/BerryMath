#include "library.h"

#include <iostream>
#include <string>
#include <vector>
#include <BerryMath.h>

BerryMath::value* number(std::vector<BerryMath::value*> arguments, std::map<string, BerryMath::value*> argumentsHash) {
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

std::vector<string> numberDoc() {
    std::vector<string> doc;
    doc.push_back("value");
    doc.push_back("type");
    return doc;
}