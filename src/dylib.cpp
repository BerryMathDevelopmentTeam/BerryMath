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
    if (dyhandle) close();
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
    // 不能即刻便销毁，不然其中的函数方法等也会被销毁，必须先存着所有句柄然后再程序结束后统一销毁
    dyhandlePool.push_back(dyhandle);
    status = false;
}
void BM::Dylib::clear()  {
    for (auto i = dyhandlePool.begin(); i != dyhandlePool.end(); i++) {
#ifdef I_OS_WIN32
        // windows
                FreeLibrary(*i);
#else
        // linux, mac, unix等
        dlclose(*i);
#endif
    }
    dyhandlePool.clear();
}