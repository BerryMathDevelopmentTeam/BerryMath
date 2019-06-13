#include "../include/value.h"

bool BM::Value::has(Value *v, Value* root = nullptr) {
    if (!root) root = this;
    if (this == v || parent == v) return true;
    if (parent == root) return false;
    if (parent && parent != this) return parent->has(v, root);
    return false;
}

void BM::Object::print(std::ostream &o, bool hl, string tab) {
    o << "{" << std::endl;
    tab += "\t";
    auto iter = proto.begin();
    auto end = proto.end();
    end--;
    for (; iter != end; iter++) {
        const string& key = iter->first;
        if (key[0] == '_' && key[1] == '_') continue;// 双下划线开头的属性不显示
        o << tab;
        if (hl) {
            o << "\033[32m\"" << key << "\"\033[0m";
        } else {
            o << "\"" << key << "\"";
        }
        o << ": ";
        if (has(iter->second)) o << "...";
        else iter->second->print(o, hl, tab);
        o << "," << std::endl;
    }
    const string& key = iter->first;
    if (key[0] != '_' && key[1] != '_') {// 双下划线开头的属性不显示
        o << tab;
        if (hl) {
            o << "\033[32m\"" << key << "\"\033[0m";
        } else {
            o << "\"" << key << "\"";
        }
        o << ": ";
        if (has(iter->second)) o << "...";
        else iter->second->print(o, hl, tab);
        o << std::endl;
    }
    tab.pop_back();
    o << tab << "}";
}
void BM::Object::set(const string &key, Value *value) {
    if (proto.count(key) == 0) {
        insert(key, value);
    } else {
        proto[key] = value;
        if (this != value) value->linked++;
        value->parent = this;
    }
}
BM::Value* BM::Object::get(const string &key) {
    auto iter = proto.find(key);
    if (iter == proto.end()) return nullptr;
    return iter->second;
}
void BM::Object::insert(string key, Value *value) {
    proto.insert(std::pair<string, Value*>(key, value));
    if (this != value) value->linked++;
    value->parent = this;
}
void BM::Object::del(const string &key) {
    auto iter = proto.find(key);
    if (iter == proto.end()) return;
    proto.erase(iter);
    Value* v = iter->second;
    v->linked--;
    if (v->linked < 1) delete v;
}
BM::Object::~Object() {
    for (auto iter = proto.begin(); iter != proto.end(); iter++) {
        Value* v = iter->second;
        if (!v || v->linked < 1) continue;
        v->linked--;
        if (v->linked < 1) delete v;
        iter->second = nullptr;
    }
    proto.clear();
}