#ifndef BERRYMATH_STACK_H
#define BERRYMATH_STACK_H

#include <map>
#include "value.h"

namespace BM {
    class Stack {
    public:
        Stack() : size(0) { }
        void push(Object*);
        void pop(size_t);
        void move(size_t, size_t);
        void load(size_t);
        Object* operator[](unsigned long long addr) { return val[addr]; }
        Object* get(unsigned long long addr) { return val[addr]; }
        ~Stack() { for (auto & i : val) i.second->unbind(); }
    private:
        // 因为这个栈是不定长的并且要是能随机访问的，虽然vector访问复杂度是O(1)，但是由于添加元素要是超过预定size, vector的插入复杂度就是O(n)了，而map虽然访问复杂度是O(log n)但是由于其插入也是稳定O(log n)所以最终选用map
        std::map<unsigned long long, Object*> val;
        size_t size;
    };
}


#endif //BERRYMATH_STACK_H
