#ifndef BERRYMATH_COMPILER_H
#define BERRYMATH_COMPILER_H

#include <string>
#include "ast.h"
#include "types.h"
#include "sym.h"
using std::string;

namespace BM {
    class Compiler {
    public:
        Compiler(const string& s = "", const string& f = "", Compiler* p = nullptr) : script(s), filename(f), parent(p), child((bool)p) {
            script += ";pass";
        }
        void open(const string& s, const string& f = "", Compiler* p = nullptr) {
            script = s + ";pass";
            filename = f;
            parent = p;
        }
        bool compile(string&);
        ~Compiler() {
            if (!child && ast) delete ast;
        }
    private:
        bool child;
        AST* ast = nullptr;
        Compiler* parent;
        Sym sym;

        string filename;
        string script;
    };
    enum Opcodes {
        PUSH,
        ADD,  SUB,  MUL,   DIV,   MOD, POW,
        AND,  OR,   XOR,   NOP,   SHL, SHR,  EQ,
        NEQ,  LT,   GT,    LE,    GE,  LAN,
        LOR,  NOT,  LNT,   MIN,   JMP,
        JMPT, JMPF, CALLA, CALLAA,
        RET,  MOV,  LOAD,
    };
    enum Optypes {
        NUM, STR, OBJ, NUL, UND
    };

#define FREEAST if (!child && ast) { delete ast;ast = nullptr; }
#define COMPILE(compiler, out) if (!compiler.compile(out)) { \
        std::clog << "\n\tat <" << filename << ">:" << ast->line() << std::endl; \
        bytecode = ""; \
        return false; \
    }
}

/*
get | reg, rstr
get | address, rstr
get | address, reg
get | reg, address
get | address1, address2
*/

#endif //BERRYMATH_COMPILER_H
