#include "library.h"
#include <random>
#include <ctime>
using std::default_random_engine;
using std::uniform_int_distribution;

Object* randint(Scope* scope, vector<Object*> unknowns) {
    default_random_engine e;
    e.seed(time(NULL));
    uniform_int_distribution<long long> u((long long)((Number*)scope->get("n"))->value(), (long long)((Number*)scope->get("m"))->value());
    return new Number(u(e));
}
Object* randfloat(Scope* scope, vector<Object*> unknowns) {
    default_random_engine e;
    e.seed(time(NULL));
    uniform_int_distribution<double> u(((Number*)scope->get("n"))->value(), ((Number*)scope->get("m"))->value());
    return new Number(u(e));
}
Object* rawrand(Scope* scope, vector<Object*> unknowns) {
    default_random_engine e;
    e.seed(time(NULL));
    uniform_int_distribution<double> u(0, 1);
    return new Number(u(e));
}

Object* initModule() {
    auto exports = new Object;
    return exports;
}