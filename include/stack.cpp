#include "stack.h"

void BM::Stack::push(BM::Object* o) {
    val[size++] = o;
    o->bind();
}
void BM::Stack::pop(size_t t) {
    size -= t;
    for (auto i = 0; i < t; i++) val[i + size]->unbind();
}
void BM::Stack::load(size_t origin) {
    if (origin > 0 && origin > 0 < size) push(val[origin]);
}
void BM::Stack::move(size_t origin, size_t target) {
    if (origin > 0 && origin > 0 < size && target > 0 && target > 0 < size) val[target] = val[origin];
    val[origin]->bind();
}