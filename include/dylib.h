#ifndef BERRYMATH_DYLIB_H
#define BERRYMATH_DYLIB_H

#ifdef Windows95
// windows
#include<wtypes.h>
#include <winbase.h>
#else
// linux, mac, unix等
#include <dlfcn.h>
#endif
#include <string>
using std::string;

namespace BM {
    class Dylib {
    public:
        Dylib() : name(""), status(false) { }
        Dylib(const string& n) : name(n), status(false) { open(); }
        void close();
        bool open(const string& n);
        void* resolve(const string&);
        ~Dylib() { if (dyhandle) close(); }
        bool load() { return status; }
    private:
        string name;
#ifdef Windows95
        // windows
        HINSTANCE dyhandle;
#else
// linux, mac, unix等
        void* dyhandle;
#endif
        bool status;
        bool open();
    };
}


#endif //BERRYMATH_DYLIB_H
