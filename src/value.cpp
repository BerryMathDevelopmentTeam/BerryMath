#include "../include/value.h"

bool BM::Object::has(Object *v, Object* root = nullptr) {
    if (!root) root = this;
    if (this == v || parent == v) return true;
    if (parent == root) return false;
    if (parent && parent != this) return parent->has(v, root);
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
        if (has(iter->second)) o += "...";
        else o += iter->second->toString(indent, hl, tab);
        o += ",";
        if (indent) o += '\n';
    }
    if (indent) {
        tab.pop_back();
        o += tab;
    }
    o += "}";
    return o;
}
void BM::Object::print(std::ostream &o, bool hl) {
    o << toString(true, hl) << std::endl;
}
void BM::Object::set(const string &key, Object *value) {
    if (proto.count(key) == 0) {
        insert(key, value);
    } else {
        proto[key] = value;
        if (this != value) value->linked++;
        value->parent = this;
    }
}
BM::Object* BM::Object::get(const string &key) {
    auto iter = proto.find(key);
    if (iter == proto.end()) return nullptr;
    return iter->second;
}
void BM::Object::insert(string key, Object *value) {
    proto.insert(std::pair<string, Object*>(key, value));
    if (this != value) value->linked++;
    value->parent = this;
}
void BM::Object::del(const string &key) {
    auto iter = proto.find(key);
    if (iter == proto.end()) return;
    proto.erase(iter);
    Object* v = iter->second;
    v->linked--;
    if (v->linked < 1) delete v;
}
BM::Object::~Object() {
    for (auto iter = proto.begin(); iter != proto.end(); iter++) {
        Object* v = iter->second;
        if (!v || v->linked < 1 || has(v)) continue;
        v->linked--;
        if (v->linked < 1) delete v;
        iter->second = nullptr;
    }
    proto.clear();
}

BM::Scope::Scope(Scope *p) : parent(p) {
    set(new Variable(SCOPE_D_NAME, new Object));
}
void BM::Scope::set(Variable *variable) {
    auto name = variable->name();
    auto iter = variables.find(name);
    if (iter == variables.end()) variables.insert(std::pair<string, Variable*>(name, variable));
    variables[name] = variable;
    if (name != SCOPE_D_NAME) {
        get(SCOPE_D_NAME, SELF)->value()->insert(name, variable->value());
    }
}
BM::Variable* BM::Scope::get(string name, Flag flag) {
    auto iter = variables.find(name);
    if (iter == variables.end()) return nullptr;
    return iter->second;
}
void BM::Scope::del(string name) {
    auto v = get(name);
    if (v) delete v;
    variables.erase(name);
    get(SCOPE_D_NAME, SELF)->value()->del(name);
}