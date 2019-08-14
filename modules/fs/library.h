#ifndef FS_LIBRARY_H
#define FS_LIBRARY_H

#include <BerryMath.h>
#include <vector>
#include <map>
using std::vector;
using std::map;
using BM::Object;
using BM::Scope;
using BM::String;
using BM::Number;
using BM::NativeFunction;
using BM::Function;
using BM::NativeValue;

Object* FileCtor(Scope*, vector<Object*>);
Object* FileWrite(Scope*, vector<Object*>);
Object* FileRead(Scope*, vector<Object*>);
Object* FileReadline(Scope*, vector<Object*>);
Object* FileReadlines(Scope*, vector<Object*>);
Object* FileReset(Scope*, vector<Object*>);
Object* FileClose(Scope*, vector<Object*>);
Object* FileRemove(Scope*, vector<Object*>);
Object* mkdir(Scope*, vector<Object*>);
Object* copy(Scope*, vector<Object*>);
Object* move(Scope*, vector<Object*>);
Object* remove(Scope*, vector<Object*>);
extern "C" Object* initModule();

#endif