#ifndef BERRYMATH_VM_H
#define BERRYMATH_VM_H

#include <string>
#include <vector>
#include "types.h"
#include "register.h"
using std::string;
using std::vector;

namespace BM {
    const qbyte MAGIC_CODE = 0xacde07fd;
    class VM;
    typedef void (*VMOP)(ULL, string&, vector<ebyte>&, vector<byte*>&, VM*);
    class VM {
    public:
        VM(const string& bc = "") : bytecode(bc) {  }
        void run();
        void open(const string& bc) { bytecode = bc; }
        string bc() { return bytecode; }
        Register regs[24];
        static void initops(vector<VMOP>&);
    private:
        string bytecode;
    };
    void Throw(const string&, const string&);
    void ThrowExit(const string&, const string&);
}


#endif //BERRYMATH_VM_H
