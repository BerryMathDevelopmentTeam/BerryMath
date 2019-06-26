#ifdef Windows95
// windows
#include<wtypes.h>
#include <winbase.h>
#else
// linux, mac, unix等
#include <dlfcn.h>
#endif
#include "Dylib.h"

bool BM::Dylib::open() {
#ifdef Windows95
    // windows
    dyhandle = LoadLibrary(name.c_str());
#else
    // linux, mac, unix等
    dyhandle = dlopen(name.c_str(), RTLD_NOW);
#endif
    return (status = (bool)dyhandle);
}
void* BM::Dylib::resolve(const string& sym) {
#ifdef Windows95
    // windows
    return (void*)GetProcAddress(dyhandle, sym.c_str());
#else
    // linux, mac, unix等
    return dlsym(dyhandle, sym.c_str());
#endif
}
void BM::Dylib::close() {
#ifdef Windows95
    // windows
    FreeLibrary(dyhandle);
#else
    // linux, mac, unix等
    dlclose(dyhandle);
#endif
    status = false;
}