#include "library.h"

#include <string>
#include <vector>
#include <map>
#include <sys/time.h>
#include <ctime>

int64_t getCurrentTime() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
tm* localtime() {
    time_t tt;
    time(&tt);
    return localtime(&tt);
}

Object* timestampMS(Scope* scope, vector<Object*> unknowns) {
    return new Number(getCurrentTime());
}
Object* timestamp(Scope* scope, vector<Object*> unknowns) {
    return new Number(time(NULL));
}
Object* year(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_year + 1900);
}
Object* month(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_mon + 1);
}
Object* day(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_wday + 1);
}
Object* hour(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_hour);
}
Object* minute(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_min);
}
Object* second(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_sec);
}
Object* date(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_mday);
}
Object* yearDay(Scope* scope, vector<Object*> unknowns) {
    return new Number(localtime()->tm_yday);
}
Object* sleep(Scope* scope, vector<Object*> unknowns) {
    unsigned long long ms = (unsigned long long)((Number*)scope->get("ms"))->value();
#ifdef I_OS_WIN
    Sleep(ms);
#else
    sleep(ms);
#endif
    return new BM::Undefined;
}

Object* initModule() {
    auto exports = new Object;

    auto timestampMSP = new NativeFunction("timestampMS", timestampMS);
    auto timestampP = new NativeFunction("timestamp", timestamp);
    auto yearP = new NativeFunction("year", year);
    auto monthP = new NativeFunction("month", month);
    auto dayP = new NativeFunction("day", day);
    auto hourP = new NativeFunction("hour", hour);
    auto minuteP = new NativeFunction("minute", minute);
    auto secondP = new NativeFunction("second", second);
    auto dateP = new NativeFunction("date", date);
    auto yearDayP = new NativeFunction("yearDay", yearDay);

    exports->set("timestampMS", timestampMSP);
    exports->set("timestamp", timestampP);
    exports->set("year", yearP);
    exports->set("month", monthP);
    exports->set("day", dayP);
    exports->set("hour", hourP);
    exports->set("minute", minuteP);
    exports->set("second", secondP);
    exports->set("date", dateP);
    exports->set("minute", minuteP);
    exports->set("yearDay", yearDayP);

    return exports;
}