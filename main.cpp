#include <iostream>
#include <cstdlib>
#include <BerryMath.h>
#include <vector>
#include <fstream>
using std::cout;
using std::endl;
using std::cin;
using std::fstream;
using std::vector;

/*void* operator new(size_t n) {
    auto o = malloc(n);
    std::cout << "n: " << o << std::endl;
    return o;
}
void operator delete(void* o) {
    std::cout << "d: " << o << std::endl;
    free(o);
}*/

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
    vector<BM::Object*> gPool;

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
            gPool.push_back(e);
        } else {
            ip.open(tmp, "terminal");
            auto e = ip.run();
            auto ret = e->get(PASS_RETURN);
            if (ret) cout << (*ret) << endl;
            gPool.push_back(e);
        }
    }
    ip.clear();
    for (auto i = gPool.begin(); i != gPool.end(); i++) {
        delete *i;
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
            bytecode += line + "\n";
        }
        BM::VM vm(bytecode);
        vm.run();
        file.close();
    } else if (opt == "--compile" || opt == "-c") {// 运行字节码
        if (argc < 3) {
            std::cerr << "SystemError: Filename not found at <null:\033[33msystem\033[0m>:0" << endl;
            exit(1);
        }
        fstream file;
        string filename(argv[2]);
        file.open(filename);
        if (!file) {
            std::cerr << "SystemError: Cannot open source file " << filename << " at <" << filename << ":\033[33msystem\033[0m>:0" << endl;
            file.close();
            exit(1);
        }
        string line;
        string source;
        while (getline(file, line)) {
            source += line + "\n";
        }
        file.close();
        BM::Compiler compiler;
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
        file.close();
        BM::Interpreter ip(script, filename);
        delete ip.run();
    }
    BM::Dylib::clear();
    return 0;
}