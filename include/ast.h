#ifndef BERRYMATH_AST_H
#define BERRYMATH_AST_H

#include <string>
#include <vector>
#include "lex.h"
using std::string;
using std::vector;

namespace BM {
    class ast {
    public:
        ast() : root(nullptr), script("") { }
        ast(string& s) : root(nullptr), script(std::move(s)) { }
        void open(string& s) { script = std::move(s); }
        void parse();
        void clear() {
            if (root) delete root;
            root = nullptr;
        }
        ~ast() { delete root; }
    public:
        class node {
        public:
            node() : v(""), l(0) { }
            node(string t) : v(t), l(0) { }
            node(string t, UL i) : v(t), l(i) { }
            inline string value() { return v; }
            inline void value(string c) { v = c; }
            inline UL line() { return l; }
            inline node* get(long index) {
                if (index < 0) index += children.size();
                return children[index % children.size()];
            }
            node& operator[](long index) { return *get(index); }
            void insert(node* n) { children.push_back(n); }
            void insert(string v, UL l) { children.push_back(new node(v, l)); }
            ~node() {
                for (auto iter = children.begin(); iter != children.end(); iter++) {
                    delete (*iter);
                }
            }
        private:
            string v;
            UL l;
            vector<node*> children;
        };
        node* root;
        string script;
    };
}


#endif //BERRYMATH_AST_H
