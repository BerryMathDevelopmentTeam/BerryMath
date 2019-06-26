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
    class Object {
    public:
        Object() : linked(0), parent(nullptr) { }
        bool has(Object*, Object*);
        void set(const string &key, Object *value);
        void insert(string, Object*);
        Object* get(const string &key);
        void del(const string &key);
        Object& operator[](const string &key) { return *get(key); }
        inline UL links() { return linked; }
        inline UL bind() { return ++linked; }
        inline UL unbind() { return --linked; }
        virtual string toString(bool = true, bool = true, string = "");
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
        ~Undefined() { }
    };
}


#endif //BERRYMATH_VALUE_H
