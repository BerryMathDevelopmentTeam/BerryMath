#include <iostream>
#include <fstream>
#include <cstdio>
using std::string;
using std::fstream;

typedef unsigned char byte;

#define GET bt = (bytecode[i++] << 4) + (bytecode[i++])
#define GETP bt = (bytecode[i++] << 4) + (bytecode[i++]);printf("%02x ", bt);

double transI64D_bin(int64_t n) {
    union {
        double a;
        int64_t b;
    } tmp;
    tmp.b = n;
    return tmp.a;
}

inline void help() {
    std::cout << "OVERVIEW: BerryMath bytecode file dumper\n" << std::endl;
    std::cout << "USAGE: bcdump [options] <input bytecode file>\n" << std::endl;
    std::cout << "OPTIONS: " << std::endl;
    printf("\t--help         - Show help text\n");
    printf("\t--version, -v  - Show version\n");
    printf("\t--header, -h   - Dump the bytecode header\n");
    printf("\t--index, -i    - Dump the bytecode index content\n");
}

int main(int argc, char* argv[]) {
    bool header = false;
    bool index = false;
    argc--;
    argv++;
    if (argc < 1 || argv[0] == "--help") {
        help();
        return 0;
    }
    if (argv[0] == "-v" || argv[0] == "--version") {
        std::cout << "BerryMath Bytecode Dumper" << std::endl;
        std::cout << "v1.0.0" << std::endl;
        return 0;
    }
    argc--;
    for (int i = 0; i < argc; i++) {
        if (argv[0] == "-h" || argv[i] == "--header") header = true;
        else if (argv[i] == "-i" || argv[i] == "--index") index = true;
        else {
            std::cerr << "Error: Wrong option: " << argv[i] << std::endl;
            help();
            return 1;
        }
    }
    if (!(header || index)) return 0;
    string filename(argv[argc]);
    fstream file;
    file.open(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        file.close();
        return 1;
    }
    string line;
    string bytecode;
    while (getline(file, line)) {
        bytecode += line + "\n";
    }
    file.close();
    unsigned long i = 0;
    byte bt;
    byte magic_code[] = {0x74, 0xaf, 0xbe, 0x28};
    for (byte t = 0; t < 4; t++) {
        GET;
        if (bt != magic_code[i]) {
            std::cerr << "Error: Wrong bytecode file '" << filename << "', wrong magic code" << std::endl;
            return 1;
        }
    }
    if (header) {
        std::cout << "74 af be 28\tMagic code" << std::endl;
    }
    string sourcename;
    while (true) {
        GET;
        if (header) {
            printf("%02x ", bt);
        }
        if (bt == 0) break;
        sourcename += bt;
    }
    if (header) {
        std::cout << "\tsource name: " << sourcename << std::endl;
    }
    unsigned long long timestamp = 0;
    for (byte t = 7; t >= 0; t--) {
        GET;
        if (header) {
            printf("%02x ", bt);
        }
        timestamp += bt << t * 8;
    }
    if (header) {
        std::cout << "\tbuilt time(timestamp): " << timestamp << std::endl;
    }
    GET;
    if (header) {
        printf("%02x ", bt);
    }
    byte first = bt;
    GET;
    if (header) {
        printf("%02x ", bt);
    }
    byte vtype = first >> 4;
    if (vtype < 10 || vtype > 12) {
        std::cerr << "Error: Wrong bytecode file '" << filename << "', wrong version type" << std::endl;
        return 1;
    }
    if (header) {
        short version = (first & 0b00001111) << 8 + bt;
        switch (vtype) {
            case 10:
                std::cout << "\tOfficialVersion: ";
                break;
            case 11:
                std::cout << "\tAdvancedVersion: ";
                break;
            case 12:
                std::cout << "\tTestVersion: ";
                break;
        }
        std::cout << version << std::endl;
    }
    if (!index) return 0;
    while (true) {
        if (i >= bytecode.length()) break;
        GETP;
        switch (bt) {
            case 0:// push
            {
                GETP;
                auto type = bt;
                switch (type) {
                    case 0:
                    {
                        unsigned long long val_ = 0;
                        for (byte t = 7; t >= 0; t--) {
                            GETP;
                            val_ += bt << t * 8;
                        }
                        double val = transI64D_bin(val_);
                        std::cout << "\tpush NUM, " << val << std::endl;
                        break;
                    }
                    case 1:
                    {
                        string val;
                        while (true) {
                            GETP;
                            if (!bt) break;
                            switch (bt) {
                                case '\n':
                                    val += "\\n";
                                    break;
                                case '\t':
                                    val += "\\t";
                                    break;
                                default:
                                    val += bt;
                                    break;
                            }
                        }
                        std::cout << "\tpush STR, \"" << val << "\"" << std::endl;
                        break;
                    }
                    case 2:
                    {
                        GETP;
                        std::cout << "\tpush OBJ, 0" << std::endl;
                        break;
                    }
                    case 3:
                    {
                        GETP;
                        std::cout << "\tpush NUL, 0" << std::endl;
                        break;
                    }
                    case 4:
                    {
                        GETP;
                        std::cout << "\tpush UND, 0" << std::endl;
                        break;
                    }
                    default:
                    {
                        std::cerr << "Error: Wrong bytecode file '" << filename << "', wrong operation type" << std::endl;
                        break;
                    }
                }
                break;
            }
            case 1: {
                std::cout << "\tadd" << std::endl;
                break;
            }
            case 2: {
                std::cout << "\tsub" << std::endl;
                break;
            }
            case 3: {
                std::cout << "\tmul" << std::endl;
                break;
            }
            case 4: {
                std::cout << "\tdiv" << std::endl;
                break;
            }
            case 5: {
                std::cout << "\tmod" << std::endl;
                break;
            }
            case 6: {
                std::cout << "\tpow" << std::endl;
                break;
            }
            case 7: {
                std::cout << "\tand" << std::endl;
                break;
            }
            case 8: {
                std::cout << "\tor" << std::endl;
                break;
            }
            case 9: {
                std::cout << "\txor" << std::endl;
                break;
            }
            case 10: {
                std::cout << "\tnop" << std::endl;
                break;
            }
            case 11: {
                std::cout << "\tshl" << std::endl;
                break;
            }
            case 12: {
                std::cout << "\tshr" << std::endl;
                break;
            }
            case 13: {
                std::cout << "\teq" << std::endl;
                break;
            }
            case 14: {
                std::cout << "\tneq" << std::endl;
                break;
            }
            case 15: {
                std::cout << "\tlt" << std::endl;
                break;
            }
            case 16: {
                std::cout << "\tgt" << std::endl;
                break;
            }
            case 17: {
                std::cout << "\tle" << std::endl;
                break;
            }
            case 18: {
                std::cout << "\tge" << std::endl;
                break;
            }
            case 19: {
                std::cout << "\tlan" << std::endl;
                break;
            }
            case 20: {
                std::cout << "\tlor" << std::endl;
                break;
            }
            case 21: {
                std::cout << "\tnot" << std::endl;
                break;
            }
            case 22: {
                std::cout << "\tlnt" << std::endl;
                break;
            }
            case 23: {
                std::cout << "\tmin" << std::endl;
                break;
            }
            case 24: {
                unsigned long long address = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    address += bt << t * 8;
                }
                printf("\tjmp %08llx\n", address);
                break;
            }
            case 25: {
                unsigned long long address = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    address += bt << t * 8;
                }
                printf("\tjmpt %08llx\n", address);
                break;
            }
            case 26: {
                unsigned long long address = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    address += bt << t * 8;
                }
                printf("\tjmpf %08llx\n", address);
                break;
            }
            case 27: {
                unsigned long long address = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    address += bt << t * 8;
                }
                printf("\tcall %08llx\n", address);
                break;
            }
            case 28: {
                std::cout << "\tret" << std::endl;
                break;
            }
            case 29: {
                unsigned long long aaddress = 0, baddress = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    aaddress += bt << t * 8;
                }
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    baddress += bt << t * 8;
                }
                printf("\tmov %08llx %08llx\n", aaddress, baddress);
                break;
            }
            case 30: {
                unsigned long long address = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    address += bt << t * 8;
                }
                printf("\tload %08llx\n", address);
                break;
            }
            case 31: {
                std::cout << "\timp" << std::endl;
                break;
            }
            case 32: {
                unsigned long long line = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    line += bt << t * 8;
                }
                std::cout << "\t.LINE: " << line << std::endl;
                break;
            }
            case 33: {
                unsigned long long aaddress = 0, baddress = 0;
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    aaddress += bt << t * 8;
                }
                for (byte t = 7; t >= 0; t--) {
                    GETP;
                    baddress += bt << t * 8;
                }
                printf("\tadp %08llx %08llx\n", aaddress, baddress);
                break;
            }
            default:
                std::cerr << "Error: Wrong bytecode file '" << filename << "', wrong operation code" << std::endl;
                break;
        }
    }
    return 0;
}