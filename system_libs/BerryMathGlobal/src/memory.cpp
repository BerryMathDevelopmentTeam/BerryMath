#include <json.h>
#include <sstream>
#include <cstring>
#include <dlfcn.h>
#include "script.h"
#include "color.h"
#include "memory.h"

#define PENDING \
std::cout << "Pending development in file: " << __FILE__ << ", at line: " << __LINE__ << ", in function: " << __FUNCTION__ << std::endl;

#define ITOA_SELF(n) std::stringstream ss; \
ss << n; \
return ss.str();

std::string itoa_self(int i)
{
    ITOA_SELF(i);
}
std::string itoa_self(float i)
{
    ITOA_SELF(i);
}
std::string itoa_self(double i)
{
    ITOA_SELF(i);
}

BerryMath::parseStates BerryMath::jsonToPrototypes(const std::string& json, hash& prototypes) {
    Json::Reader reader;
    Json::Value v;
    if (reader.parse(json, v, false)) {
        Json::Value::Members members = v.getMemberNames();
        for (Json::Value::Members::iterator it = members.begin(); it != members.end(); it++) {
            Json::ValueType vt = v[*it].type();
            double tmp1 = v[*it].asDouble();
            string tmp2 = v[*it].toStyledString();
            switch (vt) {
                case Json::stringValue:
                    prototypes.insert(std::pair<string, value*>(*it, new BerryMath::value(BerryMath::STRING, "\"" + v[*it].asString() + "\"")));
                    break;
                case Json::intValue:
                case Json::uintValue:
                case Json::realValue:
                    prototypes.insert(std::pair<string, value*>(*it, new BerryMath::value(BerryMath::NUMBER, itoa_self(tmp1))));
                    break;
                default:
                    prototypes.insert(std::pair<string, value*>(*it, new BerryMath::value(BerryMath::OBJECT, tmp2)));
                    break;
            }
        }
        return SUCCESS;
    } else {
        return FAILED;
    }
}

BerryMath::value::value() : type(BerryMath::UNDEFINED), data("undefined"), use(0) { }
BerryMath::value::value(std::string d) : data(d), use(0) {
    type = BerryMath::type(d);
    if (type == OBJECT) {
        auto flag = jsonToPrototypes(d, prototypes);
        if (!flag) {// FAILED
#warning Pending development a branch. If you can already write, please complete the code here.
            PENDING
        }
    }
}
BerryMath::value::value(value& v) : type(v.type), data(v.data), use(0) { }
BerryMath::value::value(BerryMath::TYPE t, std::string d) : type(t), data(d), use(0) { }
void BerryMath::value::insert(std::string name, value* data) {
    prototypes[name] = data;
    data->use++;
}
BerryMath::value::~value() {
    for (auto i = prototypes.begin() ; i != prototypes.end() ; i++) {
        if (--i->second->use < 1) delete i->second;
    }
}
void BerryMath::value::valueOf(std::string d) {
    if (type == OBJECT) {
        auto flag = jsonToPrototypes(d, prototypes);
        if (!flag) {// FAILED
#warning Pending development a branch. If you can already write, please complete the code here.
            PENDING
        }
    } else {
        data = d;
    }
}
BerryMath::variable::variable(string n, variable& var) : name(n) {
    if (var.v->typeOf() == OBJECT) {
        var.v->use++;
        v = var.v;
    } else {
        v = new value(*var.v);
    }
}
void BerryMath::block::each(void (*f)(variable *))  {
    for (auto i = variables.begin() ; i != variables.end() ; i++) {
        f(i->second);
    }
}

BerryMath::value * BerryMath::function::run(BerryMath::script * s, std::vector<value*>& arguments, std::map<string, value*>& argumentsHash) {
    if (expand) {
        void* handle = s->library(script);
        if (!handle) {
            s->Throw(-1, "ModuleError: The dynamic link library was not loaded");
            return new value(UNDEFINED, "undefined");
        }
        std::cout << handle << std::endl;
//        ExpandFunction func = (ExpandFunction) dlsym(handle, (name).c_str());
        ExpandFunction func = (ExpandFunction) dlsym(handle, "number");
        if (!func) {
            s->Throw(-1, "NameError: Function '" + name + "' not found in dynamic link library");
            s->note(dlerror());
            return new value(UNDEFINED, "undefined");
        }
        return func(arguments, argumentsHash);
    } else {
        s->Throw(-1, "SystemError: BerryMath interpreter is not yet supported");
        return new value(UNDEFINED, "undefined");
    }
}