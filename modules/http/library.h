#ifndef HTTP_LIBRARY_H
#define HTTP_LIBRARY_H

#include <BerryMath.h>
using BM::Object;
using BM::String;
using BM::Number;
using BM::NativeFunction;
using BM::Scope;

Object* request(Scope*, vector<Object*>);
extern "C" Object* initModule();

#endif