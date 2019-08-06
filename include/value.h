#ifndef BERRYMATH_VALUE_H
#define BERRYMATH_VALUE_H
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "types.h"
using std::string;
using std::map;
using std::vector;
typedef unsigned long UL;

namespace BM {
    extern class Interpreter;
    enum ValueType {
        OBJECT, NUMBER, STRING, NULL_, UNDEFINED, FUNCTION, NATIVE_FUNCTION
    };
    class Object;
    using forEachCB = void(*)(const string&, Object*);
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
        class Iterator {
        public:
            Iterator() : bind(nullptr) { }
            Iterator(Object* b) : bind(b) { iter = bind->proto.begin(); }
            Iterator(Object& b) : bind(&b) { iter = bind->proto.begin(); }
            void rebind(Object* b) { bind = b; iter = bind->proto.begin(); }
            void rebind(Object& b) { bind = &b; iter = bind->proto.begin(); }
            Object* value() {
                return iter->second;
            }
            Object* next() {
                iter++;
                return value();
            }
            bool end() {
                return iter == bind->proto.end();
            }
            string key() {
                return iter->first;
            }
        private:
            Object* bind;
            map<string, Object*>::iterator iter;
        };
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
            if (hl) o += "\033[32m\"" + v + "\"";
            else o += v;
            if (hl) o += "\033[0m";
            return o;
        }
        ValueType type() { return STRING; }
        string& value() { return v; }
        String* trans() {
            for (UL i = 0; i < v.length(); i++) {
                if (v[i] == '\\') {
                    if (i == v.length() - 1) break;
                    char flag = v[++i];
                    string s;
                    switch (flag) {
                        case 'n':
                            s = '\n';
                            break;
                        case '\\':
                            s = '\\';
                            break;
                        case 'r':
                            s = '\r';
                            break;
                        case 't':
                            s = '\t';
                            break;
                        case '/':
                            s = '/';
                            break;
                        case '"':
                            s = '\"';
                            break;
                        case '\'':
                            s = '\'';
                            break;
                        default:
                            s = ' ';
                            break;
                    }
                    v.replace(i - 1, 2, s);
                    i--;
                }
            }
            return this;
        }
        Object* copy() { return new String(v); }
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
    class Function : public Object {
    public:
        Function(const string& n = "", const string& s = "", Interpreter* p = nullptr) : Object(), funname(n), script(s), parent(p) {
            set("name", new String(n));
        }
        virtual ValueType type() { return FUNCTION; }
        Interpreter* interpreter() { return parent; }
        void interpreter(Interpreter* p) { parent = p; }
        Object* copy() { return new Function(funname, script, parent); }
        string value() { return "Function..."; }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            if (hl) o += "\033[34m";
            o += "Function...";
            if (hl) o += "\033[0m";
            return o;
        }
        void addDesc(string d) { desc.push_back(d); }
        void defaultValue(string name, Object* v) { defaultValues.insert(std::pair<string, Object*>(name, v)); }
        virtual Object* run(vector<Object*>, map<string, Object*>);
        string functionName() {
            return funname;
        }
        ~Function() { }
    protected:
        friend class Interpreter;
        string script;
        Interpreter* parent;
        map<string, Object*> defaultValues;
        vector<string> desc;
        string funname;
    };
    class Variable {
    public:
        Variable(string t, Object* v = new Undefined) : n(t), val(v) { val->bind(); }
        Object* value() { return val; }
        string name() {
            return n;
        }
        void value(Object* v) {
            if (val && val->unbind() < 1) delete val;
            val = v;
        }
        ~Variable() { if (val && val->unbind() < 1) delete val; }
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
        Variable* get(const string& name, Flag flag = ALL_MIGHT);
        void del(const string& name);
        void clear() { variables.clear(); }
        void setParent(Scope* p) { parent = p; }
        void load(Scope*);
        Scope* getParent() { return parent; }
        ~Scope() {
            for (auto iter = variables.begin(); iter != variables.end(); iter++) {
                if (iter->second) delete iter->second;
            }
            variables.clear();
        }
#define SCOPE_D_NAME "__scope"
    private:
        map<string, Variable*> variables;
        Scope* parent;
    };
    using NativeFuncDef = BM::Object*(*)(Scope*, vector<Object*>);
    class NativeFunction : public Object {
    public:
        NativeFunction(const string& name, NativeFuncDef n, Interpreter* p = nullptr) : Object(), funname(name), native(n), parent(p) {
            set("name", new String(name));
        }
        Object* run(vector<Object*>, map<string, Object*>);
        Interpreter* interpreter() { return parent; }
        void interpreter(Interpreter* p) { parent = p; }
        Object* copy() { return new NativeFunction(funname, native, parent); }
        string value() { return "Function..."; }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            if (hl) o += "\033[34m";
            o += "Function...";
            if (hl) o += "\033[0m";
            return o;
        }
        string functionName() {
            return funname;
        }
        void addDesc(string d) { desc.push_back(d); }
        void defaultValue(string name, Object* v) { defaultValues.insert(std::pair<string, Object*>(name, v)); }
        ValueType type() { return NATIVE_FUNCTION; }
        ~NativeFunction() { }
    private:
        friend class Interpreter;
        NativeFuncDef native;
        Interpreter* parent;
        map<string, Object*> defaultValues;
        vector<string> desc;
        string funname;
    };
    string toString(Object*);
    bool isTrue(Object*);
}


#endif //BERRYMATH_VALUE_H
