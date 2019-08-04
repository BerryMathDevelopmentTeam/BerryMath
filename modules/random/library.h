#ifndef RANDOM_LIBRARY_H
#define RANDOM_LIBRARY_H

#include <BerryMath.h>
using BM::Object;
using BM::String;
using BM::Number;
using BM::NativeFunction;
using BM::Scope;

Object* randint(Scope*, vector<Object*>);
Object* randfloat(Scope*, vector<Object*>);
Object* rawrand(Scope*, vector<Object*>);
extern "C" Object* initModule();

#endif