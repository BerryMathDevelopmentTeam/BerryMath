#include "stringpp.h"

std::string BerryMath::ltrim(const std::string& s)
{
    const string WHITESPACE = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string BerryMath::rtrim(const std::string& s)
{
    const string WHITESPACE = " \n\r\t\f\v";
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string BerryMath::trim(const std::string& s)
{
    return rtrim(ltrim(s));
}