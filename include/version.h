#ifndef BERRYMATH_VERSION_H
#define BERRYMATH_VERSION_H

#include <iostream>
using std::string;

namespace BerryMath {
#define BERRYMATH_VERSION "0.0.1"
    const int MAX_VERSION_DIFF = 1000;// 总版本差不得超过1000，即如果系统版本为1.0.0, 可运行程序版本最高为1.10.0, 最低为0.90.0
    int checkVersion(string);// 计算版本
    int versionNumber(string);// 计算版本总差距
    bool canRun(string);// 检查是否可以运行
}

#endif //BERRYMATH_VERSION_H
