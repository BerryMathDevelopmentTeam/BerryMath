#ifndef BERRYMATH_VM_H
#define BERRYMATH_VM_H

#include <string>
#include "types.h"
using std::string;

namespace BM {
    const byte MAGIC_CODEA = 0xac;
    const byte MAGIC_CODEB = 0xde;
    class VM {
    public:
        VM(const string& bc = "") : bytecode(bc) {  }
        void run();
        void open(const string& bc) { bytecode = bc; }
        string bc() { return bytecode; }
    private:
        string bytecode;
    };
    void Throw(const string&, const string&);
}


#endif //BERRYMATH_VM_H
