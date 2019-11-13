#ifndef BMLANG_LIBRARY_H
#define BMLANG_LIBRARY_H

#include <BerryMath.h>
#include <vector>
#include <map>
using std::vector;
using std::map;
using BM::Object;
using BM::Scope;

Object* createArray();
extern "C" Object* initModule();

#endif //BMLANG_LIBRARY_H