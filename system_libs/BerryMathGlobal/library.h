#ifndef BERRYMATHGLOBAL_LIBRARY_H
#define BERRYMATHGLOBAL_LIBRARY_H

#include <BerryMath.h>
#include <string>
#include <vector>
using std::string;

extern "C" BerryMath::value* Number(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* String(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* Length(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* Type(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* print(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* println(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* input(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* inputraw(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);
extern "C" BerryMath::value* Exit(std::vector<BerryMath::value*>, std::map<std::string, BerryMath::value*>);

#endif