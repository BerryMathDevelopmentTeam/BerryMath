#ifndef BERRYMATH_REGISTER_H
#define BERRYMATH_REGISTER_H

#include "types.h"

namespace BM {
    class Register
    {
    public:
        Register() : data(new byte[8]), child(false) { }
        Register(byte* d) : data(d), child(true) { }
        Register(const Register& reg) : data(reg.data), child(reg.child) {}
        byte* value(byte offset = 0) { return data + offset; }
        void value(byte v, byte offset) { *(data + offset) = v; }
        ~Register() { if (!child && data)delete[] data; }
        Register& operator=(const Register& reg) = default;
        byte* operator+(short);
    private:
        unsigned char* data;
        bool child;
    };
}

#endif //BERRYMATH_REGISTER_H
