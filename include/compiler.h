#ifndef BERRYMATH_COMPILER_H
#define BERRYMATH_COMPILER_H

#include <string>
#include "ast.h"
#include "types.h"
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
        string compile();
        ~Compiler() {
            if (!child && ast) delete ast;
        }
    private:
        bool child;
        AST* ast;
        Compiler* parent;
        string filename;
        string script;
    };
    enum Opcodes {
        LOADTV, LOADSID,
        MOVRR, MOVRA, MOVAA, MOVAR,
        PUSHA, PUSHR, CALLA, CALLAO, RET,
        ADPRRSR, ADPARSR, ADPARSA, ADPRAR, ADPAAR, ADPAAA, ADPRRR, ADPARR, ADPARA,
        ADDRR,  ADDRA,  ADDAR,  ADDAA,
        SUBRR,  SUBRA,  SUBAR,  SUBAA,
        MULRR,  MULRA,  MULAR,  MULAA,
        DIVRR,  DIVRA,  DIVAR,  DIVAA,
        MODRR,  MODRA,  MODAR,  MODAA,
        POWRR,  POWRA,  POWAR,  POWAA,
        SHLRR,  SHLRA,  SHLAR,  SHLAA,
        SHRRR,  SHRRA,  SHRAR,  SHRAA,
        LESRR,  LESRA,  LESAR,  LESAA,
        MORRR,  MORRA,  MORAR,  MORAA,
        ELESRR, ELESRA, ELESAR, ELESAA,
        EMORRR, EMORRA, EMORAR, EMORAA,
        EQRR,   EQRA,   EQAR,   EQAA,
        NEQRR,  NEQRA,  NEQAR,  NEQAA,
        ANDRR,  ANDRA,  ANDAR,  ANDAA,
        ORRR,   ORRA,   ORAR,   ORAA,
        XORRR,  XORRA,  XORAR,  XORAA,
        LANDRR, LANDRA, LANDAR, LANDAA,
        LORRR,  LORRA,  LORAR,  LORAA,
        NOTR,   NOTA,
        LNOTR,  LNOTA,
        GETRS,  GETARS, GETAR,  GETRA, GETAA,
        JMP,    JMPC,
        IMPRS,  IMPR,   IMPA,
        BITR,   BITA,
        EITR,   EITA,
        KEYR,   KEYA,
        VALR,   VALA,
        NXTR,   NXTA,
        PRER,   PREA,
        NOP,
    };
}

/*
get | reg, rstr
get | address, rstr
get | address, reg
get | reg, address
get | address1, address2
*/

#endif //BERRYMATH_COMPILER_H
