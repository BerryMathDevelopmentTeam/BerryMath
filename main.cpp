#include <iostream>
#include <fstream>
#include <BerryMath.h>

int main() {
//    BerryMath::script s("foo= 1 + 5 * 2;");
    std::ifstream in("tests/if.bm");
    string script("");
    string tmp;
    if (!in.is_open()) {
        std::cerr << RED << "SystemError: Opening 'tests/variable.bm' failed." << RESET << std::endl;
        exit(1);
    }
    while (getline(in, tmp)) {
        script += tmp + "\n";
    }
//    std::cout << script << std::endl;
    BerryMath::script s(script);
//    BerryMath::script s("foo = 1 + 5;bar = foo* 2;a=bar%10;b=a-10;c=b/4;");
    s.run();
    return 0;
}