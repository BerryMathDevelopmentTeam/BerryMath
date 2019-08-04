#ifdef I_OS_WIN32
// windows
#include<wtypes.h>
#include <winbase.h>
#else
// linux, mac, unix等
#include <dlfcn.h>
#endif
#include "dylib.h"

bool BM::Dylib::open() {
#ifdef I_OS_WIN32
    // windows
    dyhandle = LoadLibrary((name + ".dll").c_str());
#else
    // linux, mac, unix等
#ifdef __MAC_10_0
    dyhandle = dlopen((name + ".dylib").c_str(), RTLD_NOW);
#else
    dyhandle = dlopen((name + ".so").c_str(), RTLD_NOW);
#endif
#endif
    return (status = (bool)dyhandle);
}
void* BM::Dylib::resolve(const string& sym) {
#ifdef I_OS_WIN32
    // windows
    return (void*)GetProcAddress(dyhandle, sym.c_str());
#else
    // linux, mac, unix等
    return dlsym(dyhandle, sym.c_str());
#endif
}
void BM::Dylib::close() {
#ifdef I_OS_WIN32
    // windows
    FreeLibrary(dyhandle);
#else
    // linux, mac, unix等
//    dlclose(dyhandle);
#endif
    status = false;
}