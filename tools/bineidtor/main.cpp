#include <iostream>
#include <fstream>
using std::string;

int main(int argc, char* argv[]) {
    argc--, argv++;
    if (argc < 2) return 1;
    string bin, bytecode, bytecodeFilename(argv[0]);
    std::ofstream bytecodeFile(bytecodeFilename);
    if (!bytecodeFile.is_open()) return 2;
    while (!std::cin.eof()) {
        string n;
        string tmp;
        std::cin >> n;
        unsigned long long num = 0;
        for (long long i = n.length() - 1; ~i; i--) {
            num |= (n[i] - '0') << i;
        }
        do {
            tmp.insert(tmp.begin(), num & 255);
            num >>= 8;
        } while (num);
        bytecode += tmp;
    }
    bytecodeFile << bytecode;
    bytecodeFile.close();
    return 0;
}