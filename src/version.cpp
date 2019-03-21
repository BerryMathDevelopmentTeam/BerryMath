#include <vector>
#include "version.h"
#include "stringpp.h"

int BerryMath::versionNumber(string version) {
    auto pVector = spilt(version, ".");
    return atoi(pVector[0].c_str()) * 10000 + atoi(pVector[1].c_str()) * 100 + atoi(pVector[2].c_str());// xx.xx.xx 第n位的权重为10^2(2 - n)
}

int BerryMath::checkVersion(string programVersion) {
    return versionNumber(programVersion) - versionNumber(BERRYMATH_VERSION);
}

bool BerryMath::canRun(string programVersion) {
    int diff = checkVersion(programVersion);
    if (diff < 0) diff *= -1;
    return diff < MAX_VERSION_DIFF;
}