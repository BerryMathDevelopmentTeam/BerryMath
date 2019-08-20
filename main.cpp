#include <iostream>
#include <cstdlib>
// #define DEBUG
#include <BerryMath.h>
#include <fstream>
using std::cout;
using std::endl;
using std::cin;
using std::fstream;

#ifdef DEBUG
inline void* operator new(size_t sz) {
    void* o = malloc(sz);
//    std::cout << "\033[36m[LOG] \033[32mnew value: " << (long)o << "\033[0m" << std::endl;
    std::cout << "n: " << o << std::endl;
    return o;
}
inline void operator delete(void *o) {
//    std::cout << "\033[36m[LOG] \033[31mdelete value: " << (long)o << "\033[0m" << std::endl;
    std::cout << "d: " << o << std::endl;
    free(o);
}
#endif

void terminal() {
    cout << "BerryMath Terminal" << endl;
    cout << BMVersion << endl;
    cout << "        \033[32m_____\033[0m" << endl;
    cout << "         \033[32m/\\\033[0m" << endl;
    cout << "        \033[32m/  |\033[0m" << endl;
    cout << "    \033[41m     \033[0m  \033[45m   \033[0m" << endl;
    cout << "   \033[41m       \033[45m     \033[0m" << endl;
    cout << "  \033[41m   \033[35mB\033[0m\033[41m    \033[45m  \033[31mM\033[0m\033[45m  \033[0m" << endl;
    cout << "   \033[41m      \033[45m     \033[0m" << endl;
    cout << "     \033[41m    \033[45m    \033[0m" << endl;
    BM::Interpreter ip("", "terminal");

    while (true) {
        string tmp;
        cout << ">> ";
        getline(cin, tmp);
        if (tmp == ".exit") break;
        UL BBC = 0;
        UL MBC = 0;
        UL SBC = 0;
        bool trans = false;
        bool inString = false;
        for (UL i = 0; i < tmp.length(); i++) {
            if ((tmp[i] == '"' || tmp[i] == '\'') && !trans) {
                inString = !inString;
            }
            if (tmp[i] == '\\') {
                trans = !trans;
            } else {
                trans = false;
            }
            if (!inString) {
                switch (tmp[i]) {
                    case '{': BBC++;break;
                    case '}': BBC--;break;
                    case '[': MBC++;break;
                    case ']': MBC--;break;
                    case '(': SBC++;break;
                    case ')': SBC--;break;
                }
            }
        }
        if (BBC || MBC || SBC) {// 未输入完
            string s(tmp);
            while (true) {
                cout << ">> ";
                getline(cin, tmp);
                BBC = 0;
                MBC = 0;
                SBC = 0;
                for (UL i = 0; i < tmp.length(); i++) {
                    switch (tmp[i]) {
                        case '{': BBC++;break;
                        case '}': BBC--;break;
                        case '[': MBC++;break;
                        case ']': MBC--;break;
                        case '(': SBC++;break;
                        case ')': SBC--;break;
                    }
                }
                if (!(BBC || MBC || SBC)) break;
            }
            ip.open(s, "terminal");
            auto e = ip.run();
            auto ret = e->get(PASS_RETURN);
            if (ret) cout << (*ret) << endl;
            delete e;
        } else {
            ip.open(tmp, "terminal");
            auto e = ip.run();
            auto ret = e->get(PASS_RETURN);
            if (ret) cout << (*ret) << endl;
            delete e;
        }
    }
    cout << "bye" << endl;
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc == 1) terminal();
    string opt(argv[1]);
    if (opt == "--version" || opt == "-v") {// 版本
        cout << BMVersion << endl;
    } else if (opt == "--help" || opt == "-h") {// 帮助
        cout << "See https://github.com/BerryMathDevelopmentTeam/BerryMath" << endl;
    } else if (opt == "--bc" || opt == "-b") {// 运行字节码
        if (argc < 3) {
            std::cerr << "SystemError: Filename not found at <null:\033[33msystem\033[0m>:0" << endl;
            exit(1);
        }
        fstream file;
        string filename(argv[2]);
        file.open(filename);
        if (!file) {
            std::cerr << "SystemError: Cannot open bytecode file " << filename << " at <" << filename << ":\033[33msystem\033[0m>:0" << endl;
            file.close();
            exit(1);
        }
        string line;
        string bytecode;
        while (getline(file, line)) {
            bytecode += line;
        }
        BM::VM vm(bytecode);
        vm.run();
        file.close();
    } else {// 运行源码
        fstream file;
        string& filename = opt;
        file.open(filename);
        if (!file) {
            std::cerr << "SystemError: Cannot open src file " << filename << " at <" << filename << ":\033[33msystem\033[0m>:0" << endl;
            file.close();
            exit(1);
        }
        string line;
        string script;
        while (getline(file, line)) {
            script += line + "\n";
        }
        BM::Interpreter ip(script, filename);
        ip.run();
        file.close();
    }
    BM::Dylib::clear();
    return 0;
}