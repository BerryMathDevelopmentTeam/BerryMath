#ifndef BERRYMATH_VM_H
#define BERRYMATH_VM_H

#include <string>
#include <vector>
#include "value.h"
#include "types.h"
using std::string;
using std::vector;

namespace BM {
    const qbyte MAGIC_CODE = 0xacde07fd;
    class VM;
    typedef void (*VMOP)(ULL, string&, vector<ebyte>&, vector<byte*>&, VM*);
    class VM {
    public:
        VM(const string& bc = "") : bytecode(bc) {  }
        Object* run();
        void open(const string& bc) { bytecode = bc; }
        string bc() { return bytecode; }
        static void initops(vector<VMOP>&);
    private:
        string bytecode;
    };
    void Throw(const string&, const string&);
    void ThrowExit(const string&, const string&);
}


#endif //BERRYMATH_VM_H
