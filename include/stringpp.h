#ifndef BERRYMATH_STRINGPP_H
#define BERRYMATH_STRINGPP_H

#include <iostream>
using std::string;

namespace BerryMath {
    std::string ltrim(const std::string& s);
    std::string rtrim(const std::string& s);
    std::string trim(const std::string& s);
}

#endif //BERRYMATH_STRINGPP_H
