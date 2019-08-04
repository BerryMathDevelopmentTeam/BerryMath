#ifndef TIME_LIBRARY_H
#define TIME_LIBRARY_H

#include <BerryMath.h>
#include <sys/time.h>
#ifdef I_OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#endif
using BM::Object;
using BM::String;
using BM::Number;
using BM::NativeFunction;
using BM::Scope;

int64_t getCurrentTime();
tm* localtime();
Object* timestampMS(Scope*, vector<Object*>);
Object* timestamp(Scope*, vector<Object*>);
Object* year(Scope*, vector<Object*>);
Object* month(Scope*, vector<Object*>);
Object* day(Scope*, vector<Object*>);
Object* date(Scope*, vector<Object*>);
Object* hour(Scope*, vector<Object*>);
Object* minute(Scope*, vector<Object*>);
Object* second(Scope*, vector<Object*>);
Object* yearDay(Scope*, vector<Object*>);
Object* sleep(Scope*, vector<Object*>);
extern "C" Object* initModule();

#endif