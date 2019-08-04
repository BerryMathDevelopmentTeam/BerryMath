#ifndef BERRYMATH_VM_H
#define BERRYMATH_VM_H

#include <string>
#include <vector>
#include "types.h"
using std::string;
using std::vector;

namespace BM {
    const qbyte MAGIC_CODE = 0xacde07fd;
    class VM {
    public:
        VM(const string& bc = "") : bytecode(bc) {  }
        void run();
        void open(const string& bc) { bytecode = bc; }
        string bc() { return bytecode; }
        Register regs[24];
    private:
        string bytecode;
    };
    void Throw(const string&, const string&);
    void ThrowExit(const string&, const string&);
    typedef void (*VMOP)(ULL, string&, vector<ebyte>&, vector<byte*>&);
    VMOP vmops[36];
}


#endif //BERRYMATH_VM_H
