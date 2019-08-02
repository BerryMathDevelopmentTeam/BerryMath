#include <iostream>
#include "register.h"
#include "vm.h"

void BM::VM::run() {
    UL i(2);

    // 创建寄存器，关于这一部分可以查看doc/bytecode.md的内容=

    Register regs[24];

    for (byte t = 0; t < 4; t++) {
        regs[5 * t] = Register();// 64位
        regs[5 * t + 1] = Register(regs[0] + 4);// 32位
        regs[5 * t + 2] = Register(regs[1] + 2);// 16位
        regs[5 * t + 3] = Register(regs[2] + 0);// 8位
        regs[5 * t + 4] = Register(regs[2] + 1);// 8位
    }
    regs[20] = Register();// rsp
    regs[21] = Register();// rbp
    regs[22] = Register();// cs
    regs[23] = Register();// ds

    // 验证是否为bm字节码
    if ((byte)bytecode[0] != MAGIC_CODEA || (byte)bytecode[1] != MAGIC_CODEB) {
        Throw("Bytecode", "Wrong bytecode: Wrong magic code.");
    }

    string filename;
    while (bytecode[i]) {
        filename += bytecode[i++];
    }
}

void BM::Throw(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
}