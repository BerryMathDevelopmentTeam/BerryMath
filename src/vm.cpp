#include <iostream>
#include <vector>
#include "vm.h"
#include "stack.h"
using std::vector;

BM::Object* BM::VM::run() {
    Stack stack;
    unsigned long long i = 0;
    unsigned long long op;
#define ABYTE(n) n = bytecode[i++]
#define TBYTE(n) n = (bytecode[i++] << 8) | bytecode[i++]
#define FBYTE(n) n = (bytecode[i++] << 32) | (bytecode[i++] << 16) | (bytecode[i++] << 8) | bytecode[i++]
    TBYTE(op);
    std::cout << op << std::endl;
    return stack[0];
}

void BM::Throw(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
}
void BM::ThrowExit(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
    exit(1);
}

void BM::VM::initops(vector<VMOP> &) {
}