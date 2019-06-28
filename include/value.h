#ifndef BERRYMATH_VALUE_H
#define BERRYMATH_VALUE_H
#include <iostream>
#include <string>
#include <map>
#include <sstream>
using std::string;
using std::map;
typedef unsigned long UL;

namespace BM {
    enum ValueType {
        OBJECT, NUMBER, STRING, NULL_, UNDEFINED
    };
    class Object {
    public:
        Object() : linked(0), parent(nullptr) { }
        bool has(Object*, Object*);
        void set(const string &key, Object *value);
        void insert(const string&, Object*);
        Object* get(const string &key);
        void del(const string &key);
        Object& operator[](const string &key) { return *get(key); }
        inline UL links() { return linked; }
        inline UL bind() { return ++linked; }
        inline UL unbind() { return --linked; }
        virtual string toString(bool = true, bool = true, string = "");
        virtual ValueType type() { return OBJECT; }
        virtual Object* copy() {
            auto object = new Object();
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                object->set(iter->first, iter->second->copy());
            }
            return object;
        }
        virtual ~Object();
        friend std::ostream& operator<<(std::ostream& o, Object& v) {
            v.print(o);
            return o;
        }
    protected:
        void print(std::ostream&, bool = true);
        UL linked;
        map<string, Object*> proto;
        Object* parent;
    };
    class Number : public Object {
    public:
        Number() : Object(), v(0) { }
        Number(double t) : Object(), v(t) { }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            std::ostringstream oss;
            oss << v;
            if (hl) o += "\033[33m";
            o += oss.str();
            if (hl) o += "\033[0m";
            return o;
        }
        ValueType type() { return NUMBER; }
        double& value() { return v; }
        Object* copy() {
            return new Number(v);
        }
        ~Number() { }
    private:
        double v;
    };
    class String : public Object {
    public:
        String() : Object(), v("") { }
        String(const string& t) : Object(), v(t) { }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            if (hl) o += "\033[32m";
            o += "\"" + v + "\"";
            if (hl) o += "\033[0m";
            return o;
        }
        ValueType type() { return STRING; }
        string& value() { return v; }
        Object* copy() {
            return new String(v);
        }
        ~String() { }
    private:
        string v;
    };
    class Null : public Object {
    public:
        Null() : Object() { }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            if (hl) o += "\033[36m";
            o += "null";
            if (hl) o += "\033[0m";
            return o;
        }
        Object* copy() {
            return new Null;
        }
        string value() {
            return "null";
        }
        ValueType type() { return NULL_; }
        ~Null() { }
    };
    class Undefined : public Object {
    public:
        Undefined() : Object() { }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            if (hl) o += "\033[35m";
            o += "undefined";
            if (hl) o += "\033[0m";
            return o;
        }
        Object* copy() {
            return new Undefined;
        }
        string value() {
            return "undefined";
        }
        ValueType type() { return UNDEFINED; }
        ~Undefined() { }
    };
    class Variable {
    public:
        Variable(string t, Object* v = new Undefined) : n(t), val(v) { val->bind(); }
        Object* value() { return val; }
        string name() {
            return n;
        }
        void value(Object* v) {
            if (val->unbind() < 1) delete val;
            val = v;
        }
        ~Variable() { if (val->unbind() < 1) delete val; }
    private:
        string n;
        Object* val;
    };
    class Scope {
    public:
        enum Flag {
            SELF, ALL_MIGHT
        };
        Scope(Scope* p = nullptr);
        void set(const string&, Object*);
        Variable* get(string name, Flag flag = ALL_MIGHT);
        void del(string name);
        void clear() { variables.clear(); }
        ~Scope() {
            for (auto iter = variables.begin(); iter != variables.end(); iter++) {
                delete iter->second;
            }
            variables.clear();
        }
#define SCOPE_D_NAME "__scope"
    private:
        map<string, Variable*> variables;
        Scope* parent;
    };
}


#endif //BERRYMATH_VALUE_H
