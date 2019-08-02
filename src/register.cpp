#include "register.h"

byte* BM::Register::operator+(short offset) {
    return data + offset;
}