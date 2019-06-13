#ifndef BERRYMATH_VALUE_H
#define BERRYMATH_VALUE_H
#include <iostream>
#include <string>
#include <map>
using std::string;
using std::map;

namespace BM {
    class Object;
    class Value {
    public:
        Value() : linked(0) { }
        virtual ~Value() { }
        virtual Value* copy() { return nullptr; }
        bool has(Value* v, Value* root);
        void bind() { linked++; }
        void unbind() { linked--; }
        unsigned int count() { return linked; }
    protected:
        unsigned int linked;
        Object* parent;
        friend class Object;
        virtual void print(std::ostream&, bool hl = true, string tab = "") = 0;
        friend std::ostream& operator<<(std::ostream& o, Value& v) {
            v.print(o);
            return o;
        }
    };
    class Object : public Value {
    public:
        Object() : Value() { parent = nullptr; }
        Object(Object& o) {
            parent = o.parent;
        }
        Value* copy() {
            auto object = new Object();
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                object->set(iter->first, iter->second->copy());
            }
            return object;
        }
        void insert(string key, Value* value);
        Value* get(const string& key);
        void set(const string& key, Value* value);
        void del(const string& key);
        ~Object();
    private:
        std::map<string, Value*> proto;
        void print(std::ostream& o, bool hl = true, string tab = "");
    };
    class Number : public Value {
    public:
        Number() : Value() { parent = nullptr; }
        Number(double t) : Value(), v(t) { parent = nullptr; }
        void value(double t) { v = t; }
        double& value() { return v; }
        ~Number() { }
        Value* copy() {
            return new Number(v);
        }
    private:
        void print(std::ostream& o, bool hl = true, string tab = "") {
            if (hl) {
                o << "\033[33m" << v << "\033[0m";
            } else {
                o << v;
            }}
        double v;
    };
    class String : public Value {
    public:
        String() : Value() { parent = nullptr; }
        String(string t) : Value(), v(t) { parent = nullptr; }
        void value(string t) { v = t; }
        string& value() { return v; }
        ~String() { }
        Value* copy() {
            return new String(v);
        }
    private:
        void print(std::ostream& o, bool hl = true, string tab = "") {
            if (hl) {
                o << "\033[32m\"" << v << "\"\033[0m";
            } else {
                o << "\"" << v << "\"";
            }
        }
        string v;
    };
    class Null : public Value {
    public:
        Null() : Value() { parent = nullptr; }
        ~Null() { }
        Value* copy() {
            return new Null;
        }
    private:
        void print(std::ostream& o, bool hl = true, string tab = "") {
            if (hl) {
                o << "\033[1mnull\033[0m";
            } else {
                o << "null";
            }}
    };
    class Undefined : public Value {
    public:
        Undefined() : Value() { parent = nullptr; }
        ~Undefined() { }
        Value* copy() {
            return new Null;
        }
    private:
        void print(std::ostream& o, bool hl = true, string tab = "") {
            if (hl) {
                o << "\033[35mundefined\033[0m";
            } else {
                o << "undefined";
            }}
    };
}


#endif //BERRYMATH_VALUE_H
