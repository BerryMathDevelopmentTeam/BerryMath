#include <iostream>
using std::string;
using std::cout;
using std::cin;
using std::endl;

void ILog(string txt) {
    cout << txt << endl;
}

int main() {
    ILog("BerryMath Programming Language Interpreter Installer");
    ILog("Init system...");
#ifdef Windows95
    string installPath("C:\\Program Files\\");
#else
    string installPath("/usr/local/");
#endif
    system(("mkdir " + installPath + "BM"));
    return 0;
}