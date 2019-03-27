#ifndef BERRYMATHGLOBAL_LIBRARY_H
#define BERRYMATHGLOBAL_LIBRARY_H

#include <BerryMath.h>
#include <string>
#include <vector>
using std::string;

extern "C" BerryMath::value* number(std::vector<BerryMath::value*>, std::map<string, BerryMath::value*>);
extern "C" std::vector<string> numberDoc();

#endif