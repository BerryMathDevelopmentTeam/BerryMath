#ifndef BERRYMATH_CACHE_H
#define BERRYMATH_CACHE_H

#include <map>

namespace BM {
    inline std::map<void*, bool> deleted;
}

void* operator new(size_t);
void operator delete(void*);

#endif //BERRYMATH_CACHE_H
