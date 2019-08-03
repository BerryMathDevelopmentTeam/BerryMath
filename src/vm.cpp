#include <iostream>
#include "register.h"
#include "vm.h"

void BM::VM::run() {
#define GET(n) n = bytecode[i++];
#define GETD(n) n = bytecode[i++] * 16 + bytecode[i++];
#define GETOP GETD(opId);

    UL i(0);

    // 创建寄存器，关于这一部分可以查看doc/bytecode.md的Register table的部分
    Register regs[24];
    for (byte t = 0; t < 4; t++) {
        regs[5 * t] = Register(new byte[8]);// 64位
        regs[5 * t + 1] = Register(regs[0] + 4);// 32位
        regs[5 * t + 2] = Register(regs[1] + 2);// 16位
        regs[5 * t + 3] = Register(regs[2] + 0);// 8位
        regs[5 * t + 4] = Register(regs[2] + 1);// 8位
    }
    regs[20] = Register(new byte[8]);// rsp
    regs[21] = Register(new byte[8]);// rbp
    regs[22] = Register(new byte[8]);// cs
    regs[23] = Register(new byte[8]);// ds

    // 验证是否为bm字节码
    dbyte magicCode;
    GETD(magicCode);
    if (magicCode != 0xacde) ThrowExit("Bytecode", "Wrong bytecode: Wrong magic code.");

    string filename;
    while (bytecode[i]) {
        filename += bytecode[i++];
    }
    i++;

    dbyte opId;

    GETOP;
    if (opId != 1) ThrowExit("Bytecode", "Wrong bytecode: Wrong operator.");
}

void BM::Throw(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
}
void BM::ThrowExit(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
    exit(1);
}