#include <iostream>
#include <vector>
#include "register.h"
#include "vm.h"
using std::vector;

void BM::VM::run() {
#define GET(n) n = bytecode[i++];
#define GETD(n) n = (byte)(bytecode[i++]) * 0x100 + (byte)bytecode[i++];
#define GETQ(n) n = (byte)(bytecode[i++]) * 0x1000000 + (byte)(bytecode[i++]) * 0x10000 + (byte)(bytecode[i++]) * 0x100 + (byte)bytecode[i++];
#define GETE(n) n = (byte)(bytecode[i++]) * 0x1000000000000 + (byte)(bytecode[i++]) * 0x10000000000 + (byte)(bytecode[i++]) * 0x100000000 + (byte)bytecode[i++] * 0x1000000 + (byte)(bytecode[i++]) * 0x1000000 + (byte)(bytecode[i++]) * 0x10000 + (byte)(bytecode[i++]) * 0x100 + (byte)bytecode[i++];
#define GETOP GETD(opId);

    ULL i(0);
    vector<ebyte> stk;

    // 创建寄存器，关于这一部分可以查看doc/bytecode.md的Register table的部分
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

    vector<byte*> static_pool;

    // 验证是否为bm字节码
    byte magicCode;
    GET(magicCode);
    if (magicCode != 0xac) ThrowExit("Bytecode", "Wrong bytecode: Wrong magic code.");
    GET(magicCode);
    if (magicCode != 0xde) ThrowExit("Bytecode", "Wrong bytecode: Wrong magic code.");
    GET(magicCode);
    if (magicCode != 0x07) ThrowExit("Bytecode", "Wrong bytecode: Wrong magic code.");
    GET(magicCode);
    if (magicCode != 0xfd) ThrowExit("Bytecode", "Wrong bytecode: Wrong magic code.");

    string filename;
    while (bytecode[i]) {
        filename += bytecode[i++];
    }
    i++;

    int opId;

    GETOP;
    if (opId != 0xff01) ThrowExit("Bytecode", "Wrong bytecode: Wrong operator, expected op 0xff01.");
    while (opId != 0xff02) {
        GETOP;
        if (opId == 0xff02) break;
        byte flag;
        GET(flag);
        switch (flag) {
            case 0:
            {
                byte value;
                GET(value);
                static_pool.push_back(new byte(value));
                break;
            }
            case 1:
            case 2:
            case 3:
            {
                const auto count = (flag == 1 ? 2 : (flag == 2 ? 4 : 8));
                auto pv = new byte[count];
                for (byte j = 0; j < count; j++) {
                    byte v;
                    GET(v);
                    pv[j] = v;
                }
                static_pool.push_back(pv);
                break;
            }
            default:
                ThrowExit("Bytecode", "Wrong bytecode: Wrong static data flag.");
                break;
        }
    }
    GETE(i);

    while (true) {
        GETOP;
    }
}

void BM::Throw(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
}
void BM::ThrowExit(const string& name, const string& txt) {
    std::cerr << name << "Error: " << txt << std::endl;
    exit(1);
}