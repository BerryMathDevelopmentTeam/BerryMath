#include <iostream>
#include <fstream>
#include <BerryMath.h>

int main() {
//    BerryMath::script s("foo= 1 + 5 * 2;");
    string filename("expression.bm");
    std::ifstream in("tests/" + filename);

    // 载入script
    string script("");
    string tmp;
    if (!in.is_open()) {
        std::cerr << RED << "SystemError: Opening 'tests/variable.bm' failed." << RESET << std::endl;
        exit(1);
    }
    while (getline(in, tmp)) {
        script += tmp + "\n";
    }

    // 载入system.json文件
#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED) || defined(__linux) || defined(__unix)// mac, linux, unix
    std::ifstream input("/usr/local/BerryMath/system_files/system.json");
#else // windows
    std::ifstream input("C:\\Program Files\\BerryMath\\system_files\\system.json");
#endif
    string json("");
    while (getline(input, tmp)) {
        json += tmp + "\n";
    }
//    std::cout << script << std::endl;
    BerryMath::script s(script, filename);
//    BerryMath::script s("foo = 1 + 5;bar = foo* 2;a=bar%10;b=a-10;c=b/4;");
    s.init(json);
    s.run();
    s.finish();
    return 0;
}