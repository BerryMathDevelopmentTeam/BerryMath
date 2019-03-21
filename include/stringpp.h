#ifndef BERRYMATH_STRINGPP_H
#define BERRYMATH_STRINGPP_H

#include <iostream>
#include <vector>
using std::string;

namespace BerryMath {
    std::string ltrim(const std::string& s);
    std::string rtrim(const std::string& s);
    std::string trim(const std::string& s);
    std::vector<string> spilt(const string&, const string&);
}

#endif //BERRYMATH_STRINGPP_H
