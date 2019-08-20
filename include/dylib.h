#ifndef BERRYMATH_DYLIB_H
#define BERRYMATH_DYLIB_H

#include "../OSPlatformUtil/src/osplatformutil.h"
#ifdef I_OS_WIN32
// windows
#include<wtypes.h>
#include <winbase.h>
#else
// linux, mac, unix等
#include <dlfcn.h>
#endif
#include <string>
#include <vector>
#include "types.h"

using std::string;

namespace BM {
    class Dylib {
    public:
        Dylib() : name(""), status(false) { }
        Dylib(const string& n) : name(n), status(false) { open(); }
        void close();
        bool open(const string& n) {
            name = n;
            status = false;
            return open();
        }
        void* resolve(const string&);
        ~Dylib() { if (dyhandle) close(); }
        bool load() { return status; }
    private:
        string name;
#ifdef I_OS_WIN32
        // windows
        HINSTANCE dyhandle;
#else
// linux, mac, unix等
        void* dyhandle;
#endif
        bool status;
        bool open();
    private:
        static
        inline
#ifdef I_OS_WIN32
        // windows
std::vector<HINSTANCE> dyhandlePool;
#else
// linux, mac, unix等
        std::vector<void*> dyhandlePool;
#endif
    public:
        static void clear();
    };
}


#endif //BERRYMATH_DYLIB_H
