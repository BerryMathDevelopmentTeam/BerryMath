#ifndef BERRYMATH_TYPES_H
#define BERRYMATH_TYPES_H

#include <string>
using std::string;

typedef unsigned long long ULL;
typedef unsigned long UL;
typedef unsigned U;
typedef long long LL;
typedef long L;
typedef unsigned char byte;
typedef short dbyte;
typedef int qbyte;
typedef long long ebyte;

namespace BM {
    inline bool isNumber(const string&);
    inline bool isString(const string&);
    inline double transSD(const string&);// trans string to double
    inline int64_t transDI64_bin(double);// trans double to int64_t by binary
    inline string transI64S_bin(int64_t);// trans int64_t to string by binary
}

#endif //BERRYMATH_TYPES_H
