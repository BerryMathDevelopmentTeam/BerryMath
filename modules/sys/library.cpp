#include <BerryMath.h>
#include "library.h"

BM::Object* initModule() {
    auto exports = new BM::Object;
    exports->set("PI", new BM::Number(3.1415));
    return exports;
}