#include <cstdlib>
#include <map>
#include <vector>
#include "interpreter.h"
#include "dylib.h"
#include "value.h"
using std::string;
using std::map;
using std::vector;

#define DEBUG

bool BM::Object::has(Object *v, Object* root = nullptr, bool flag) {
    if (!root) root = this;
    if (this == v || (parent == v && !flag)) return true;
    if (parent == root) return false;
    if (parent && parent != this && !flag) return parent->has(v, root, flag);
    for (auto iter = proto.begin(); iter != proto.end(); iter++) {
        if (iter->second && (iter->second == this || iter->second->has(this, root, flag))) return true;
    }
    return false;
}
bool BM::Object::delhas(Object* v) {
    if (v == this) return true;
    for (auto iter = proto.begin(); iter != proto.end(); iter++) {
        if (iter->second != this && iter->second != v && iter->second->delhas(v)) return true;
    }
    return false;
}
string BM::Object::toString(bool indent, bool hl, string tab) {
    string o("{");
    if (indent) {
        o += "\n";
        tab += "\t";
    }
    auto iter = proto.begin();
    auto end = proto.end();
    auto systemType = get(OBJECT_PASS_SYSTEM_TYPE);
    SystemStructureType t = NONE;
    if (systemType) {
        string typeStr(((String*)systemType)->value());
        if (typeStr == "Array") t = ARRAY;
    }
    switch (t) {
        case ARRAY:
        {
            o = "[";
            ULL i = 0;
            string key(std::to_string(i));
            auto v = get(key);
            if (v) {
                if (has(v)) {
                    o += "...(value)...";
                } else {
                    o += v->toString(indent, hl, tab);
                }
            }
            i++;
            for (; ; i++) {
                key = std::to_string(i);
                v = get(key);
                if (!v) break;
                if (has(v)) {
                    o += ", ...(value)...";
                } else {
                    o += ", " + v->toString(indent, hl, tab);
                }
            }
            o += "]";
            break;
        }
        case NONE:
        {
            for (; iter != end; iter++) {
                const string& key = iter->first;
                if (key[0] == '_' && key[1] == '_') continue;// 双下划线开头的属性不显示
                o += tab;
                if (hl) {
                    o += "\033[32m\"" + key + "\"\033[0m";
                } else {
                    o += "\"" + key + "\"";
                }
                o += ": ";
                auto value = iter->second;
                string valueStr(value->toString(indent, hl, tab));
//                if (value->type() == STRING && !hl) {
//                    valueStr.insert(0, "\"");
//                    valueStr += "\"";
//                }
                if (has(iter->second)) o += "...";
                else o += valueStr;
                o += ",";
                if (indent) o += '\n';
            }
            if (indent) {
                tab.pop_back();
                o += tab;
            }
            o += "}";
            break;
        }
    }
    return o;
}
void BM::Object::print(std::ostream &o, bool hl) {
    o << toString(true, hl) << std::endl;
}
void BM::Object::set(const string &key, Object *value) {
    if (proto.count(key)) {
        auto v = proto[key];
        if (v->unbind() < 1) delete v;
        proto[key] = value;
        value->bind();
        value->parent = this;
    } else {
        insert(key, value);
    }
}
BM::Object* BM::Object::get(const string &key) {
    auto iter = proto.find(key);
    if (iter == proto.end()) return nullptr;
    return iter->second;
}
void BM::Object::insert(const string& key, Object *value) {
    proto.insert(std::pair<string, Object*>(key, value));
    if (this != value) value->linked++;
    value->parent = this;
}
void BM::Object::del(const string &key) {
    auto iter = proto.find(key);
    if (iter == proto.end()) return;
    if (iter->second->unbind() < 1) delete iter->second;
    proto.erase(iter);
//    Object* v = iter->second;
//    if (v) {
//        v->linked--;
//        if (v->linked < 1) delete v;
//    }
}
BM::Object::~Object() {
#ifdef DEBUG
    std::cout << "d: " << this << std::endl;
#endif
    for (auto iter = proto.begin(); iter != proto.end(); iter++) {
        Object* v = iter->second;
        if (!v || v->linked < 1) continue;
        if (!delhas(v) && v->unbind() < 1) delete v;
    }
    proto.clear();
}

BM::Scope::Scope(Scope *p) : parent(p) {
    set(SCOPE_D_NAME, new Object);
}
BM::Variable* BM::Scope::get(const string& name, Flag flag) {
    auto iter = variables.find(name);
    if (iter == variables.end()) {
        if (flag == ALL_MIGHT && parent) {
            return parent->get(name, flag);
        }
        if (name == "this") {
            auto ret = new Object;
            set(name, ret);
            return get(name);
        } else {
            return nullptr;
        }
    }
    return iter->second;
}
void BM::Scope::del(const string& name) {
//    auto v = get(name);
//    if (v) delete v;
    variables.erase(name);
    get(SCOPE_D_NAME, SELF)->value()->del(name);
}
void BM::Scope::set(const string& name, Object* v) {
    auto iter = variables.find(name);
    if (iter == variables.end()) variables.insert(std::pair<string, Variable*>(name, new Variable(name, v)));
    else {
        if (iter->second->value()->links() < 1 || iter->second->value()->unbind() < 1) iter->second->value();
        iter->second->value(v);
        v->bind();
    }
    if (name != SCOPE_D_NAME) {
        get(SCOPE_D_NAME, SELF)->value()->set(name, v);
    }
}

BM::Object* BM::Function::run(vector<Object*> args, map<string, Object*> hash) {
    Interpreter ip(script, parent ? parent->fn() : "", parent);

    // 优先级顺序: 指定 > 传参 > 默认
    for (auto iter = defaultValues.begin(); iter != defaultValues.end(); iter++) {
        ip.set(iter->first, iter->second);
    }
    for (UL i = 0; i < args.size(); i++) {
        ip.set(i < desc.size() ? desc[i] : ("argv" + std::to_string(i - desc.size())), args[i]);
    }
    for (auto iter = hash.begin(); iter != hash.end(); iter++) {
        ip.set(iter->first, iter->second);
    }
    ip.upscope = "\033[34mfunction " + funname + "\033[0m";
    auto exports = ip.run();
    if (exports->get(PASS_ERROR)) THROW;
    return exports->get(PASS_RETURN);
}
BM::Object * BM::NativeFunction::run(vector<Object *> args, map<string, Object *> hash) {
    auto s = new Scope(parent ? parent->scope : nullptr);
    vector<Object*> unknowns;

    // 优先级顺序: 指定 > 传参 > 默认
    for (auto iter = defaultValues.begin(); iter != defaultValues.end(); iter++) {
        s->set(iter->first, iter->second);
    }
    for (UL i = 0; i < args.size(); i++) {
        if (i < desc.size()) s->set(desc[i], args[i]);
        else unknowns.push_back(args[i]);
    }
    for (auto iter = hash.begin(); iter != hash.end(); iter++) {
        s->set(iter->first, iter->second);
    }
    return native(s, unknowns);
}
bool BM::isTrue(Object* o) {
    if (o->type() == OBJECT || o->type() == FUNCTION || o->type() == NATIVE_FUNCTION || o->type() == STRING) return true;
    if (o->type() == NULL_ || o->type() == UNDEFINED) return false;
    auto v = ((Number*)o)->value();
    return (bool)v;
}
void BM::Scope::load(Scope* p) {
    for (auto iter = p->variables.begin(); iter != p->variables.end(); iter++) {
        set(iter->first, iter->second->value());
    }
}
BM::Object* BM::Function::copy() {
    auto fun = new Function(funname, script, parent);
    for (auto iter = desc.begin(); iter != desc.end(); iter++) {
        fun->addDesc(*iter);
    }
    for (auto iter = defaultValues.begin(); iter != defaultValues.end(); iter++) {
        fun->defaultValue(iter->first, iter->second);
    }
    return fun;
}
BM::Object* BM::NativeFunction::copy() {
    auto fun = new NativeFunction(funname, native, parent);
    for (auto iter = desc.begin(); iter != desc.end(); iter++) {
        fun->addDesc(*iter);
    }
    for (auto iter = defaultValues.begin(); iter != defaultValues.end(); iter++) {
        fun->defaultValue(iter->first, iter->second);
    }
    return fun;
}