#ifndef BERRYMATH_VALUE_H
#define BERRYMATH_VALUE_H
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <list>
#include "types.h"
#include "dylib.h"
using std::string;
using std::map;
using std::vector;

namespace BM {
    extern class Interpreter;
    enum ValueType {
        OBJECT, NUMBER, STRING, NULL_, UNDEFINED, FUNCTION, NATIVE_FUNCTION, NATIVE_VALUE
    };
    enum SystemStructureType {
        NONE, ARRAY
    };
    class Object;
    inline void clearObject(Object* obj);
    using forEachCB = void(*)(const string&, Object*);
    class Object {
    public:
        Object() : linked(0) {  }
        bool has(Object*, Object* = nullptr, bool = true);
        bool delhas(Object*);
        void set(const string &key, Object *value);
        void insert(const string&, Object*);
        Object* get(const string &key);
        void del(const string &key);
        Object& operator[](const string &key) { return *get(key); }
        inline LL links() { return linked; }
        inline LL bind() {
            if (linked >= 0) return ++linked;
            return linked;
        }
        inline LL unbind() {
            if (linked == 0) clearObject(this);
            return --linked;
        }
        inline bool empty() { return proto.empty(); }
        virtual string toString(bool = true, bool = true, string = "");
        virtual ValueType type() { return OBJECT; }
        virtual Object* copy() {
            auto object = new Object();
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                object->set(iter->first, iter->second->copy());
            }
            return object;
        }
        virtual vector<string> memberNames() {
            vector<string> ret;
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                ret.push_back(iter->first);
            }
            return ret;
        }
        virtual unsigned long long memberCount() { return proto.size(); }
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
        LL linked;
        map<string, Object*> proto;
        map<Object*, bool> parents;
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
        ~Number() {
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v)) v->unbind();
            }
            proto.clear();
        }
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
            for (LL i = 0; i < v.length(); i++) {
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
        ~String() {
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v)) v->unbind();
            }
            proto.clear();
        }
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
        ~Null() {
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v)) v->unbind();
            }
            proto.clear();
        }
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
        ~Undefined() {
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v)) v->unbind();
            }
            proto.clear();
        }
    };
    class Function : public Object {
    public:
        Function(const string& n = "", const string& s = "", Interpreter* p = nullptr) : Object(), funname(n), script(s), parent(p) {
            set("name", new String(n));
        }
        virtual ValueType type() { return FUNCTION; }
        Interpreter* interpreter() { return parent; }
        void interpreter(Interpreter* p) { parent = p; }
        Object* copy();
        string value() { return "Function..."; }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o("");
            if (hl) o += "\033[34m";
            o += "Function...";
            if (hl) o += "\033[0m";
            return o;
        }
        void addDesc(string d) { desc.push_back(d); }
        void addRefer(string d) { refers[d] = true; }
        bool isRefer(string d) { return refers.count(d) > 0; }
        void defaultValue(string name, Object* v) { v->bind();defaultValues.insert(std::pair<string, Object*>(name, v)); }
        void setParent(Interpreter* ip) { parent = ip; }
        Interpreter* getParent() { return parent; }
        virtual Object* run(vector<Object*>, map<string, Object*>);
        string functionName() {
            return funname;
        }
        void functionName(const string& n) {
            funname = n;
        }
        ~Function() {
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v)) v->unbind();
            }
            for (auto iter = defaultValues.begin(); iter != defaultValues.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v)) v->unbind();
            }
            proto.clear();
        }
    protected:
        friend class Interpreter;
        string script;
        Interpreter* parent;
        map<string, Object*> defaultValues;
        map<string, bool> refers;
        vector<string> desc;
        string funname;
    };
    class Variable {
    public:
        Variable(const string& t, Object* v = new Undefined) : n(t), val(v) { val->bind(); }
        Object* value() { return val; }
        string name() {
            return n;
        }
        void value(Object* v) {
            if (val)
                val->unbind();
            val = v;
            val->bind();
        }
        ~Variable() {
            if (val) val->unbind();
        }
    private:
        string n;
        Object* val;
    };
    class Scope {
    public:
        enum Flag {
            SELF, ALL_MIGHT
        };
#define SCOPE_D_NAME "__scope"
        Scope(Scope* p = nullptr);
        void set(const string&, Object*);
        Variable* get(const string&, Flag = ALL_MIGHT);
        void del(const string& name);
        void clear() { variables.clear(); }
        void setParent(Scope* p) { parent = p; }
        void load(Scope*);
        void stack() {
            if (variables.empty()) return;
            std::cout << "[SCOPE] {" << std::endl;
            for (auto iter = variables.begin(); iter != variables.end(); iter++) {
                if (iter->second && iter->second->value()->links()) {
                    std::cout << "\t\033[32m" << iter->first << "\033[0m<variable at \033[33m" << ((void*)(iter->second)) << "\033[0m, value at \033[33m" << ((void*)(iter->second->value())) << "\033[0m>: " << iter->second->value()->toString(true, true, "\t") << std::endl;
                }
            }
            std::cout << "}" << std::endl;
        }
        Scope* getParent() { return parent; }
        ~Scope() {
            auto __scope = get(SCOPE_D_NAME);
            if (__scope) delete __scope;
            variables.erase(variables.find(SCOPE_D_NAME));
            for (auto iter = variables.begin(); iter != variables.end(); iter++) {
                if (iter->second) delete iter->second;
            }
            variables.clear();
        }
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
        Object* copy();
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
        Interpreter* getParent() { return parent; }
        void setParent(Interpreter* p) { parent = p; }
        void addDesc(const string& d) { desc.push_back(d); }
        void defaultValue(const string& name, Object* v) { defaultValues.insert(std::pair<string, Object*>(name, v)); v->bind(); }
        ValueType type() { return NATIVE_FUNCTION; }
        ~NativeFunction() {
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v) && v->unbind() == 0) delete v;
            }
            proto.clear();
            for (auto i = defaultValues.begin(); i != defaultValues.end(); i++) {
                i->second->unbind();
            }
            defaultValues.clear();
            desc.clear();
        }
    private:
        friend class Interpreter;
        NativeFuncDef native;
        Interpreter* parent;
        map<string, Object*> defaultValues;
        vector<string> desc;
        string funname;
    };
    typedef void* nativeValueType;
    class NativeValue : public Object {
    public:
        NativeValue(nativeValueType t = nullptr) : Object(), nv(t) { }
        Object* copy() { return new NativeValue(nv); }
        ValueType type() { return NATIVE_VALUE; }
        nativeValueType& value() { return nv; }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o;
            if (hl) o += "\033[36m";
            o += "NativeValue...";
            if (hl) o += "\033[0m";
            return o;
        }
        ~NativeValue() {  
            for (auto iter = proto.begin(); iter != proto.end(); iter++) {
                Object* v = iter->second;
                if (!v || v->links() < 1) continue;
                if (!delhas(v) && v->unbind() == 0) delete v;
            }
            proto.clear();
        }
    private:
        nativeValueType nv;
    };
//    BM::Object*(*)(Scope*, vector<Object*>)
    class NativeClass : public Object {
    public:
        NativeClass(const string& name) : Object() {
            auto proto = new Object;
            set("prototype", proto);
            set("name", new String(name));
            proto->set("__type__", new String(name));
        }
        void addStatic(const string& name, Object* v) {
            set(name, v);
        }
        void addProto(const string& name, Object* v) {
            get("prototype")->set(name, v);
        }
        ValueType type() { return OBJECT; }
        string toString(bool = true, bool hl = true, string tab = "") {
            string o;
            if (hl) o += "\033[36m";
            o += "[Native Clase] class { [native]... }";
            if (hl) o += "\033[0m";
            return o;
        }
    };
    string toString(Object*);
    bool isTrue(Object*);

#define OBJECT_PASS_SYSTEM_TYPE "__SYSTEM_TYPE__"
}


#endif //BERRYMATH_VALUE_H
