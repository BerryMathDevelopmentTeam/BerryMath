#ifndef SYS_LIBRARY_H
#define SYS_LIBRARY_H

#include <BerryMath.h>
#include <vector>
#include <map>
using std::vector;
using std::map;
using BM::Object;
using BM::Scope;

Object* print(Scope* scope, vector<Object*>);
extern "C" Object* initModule();

#endif