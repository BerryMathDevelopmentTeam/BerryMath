#ifndef BERRYMATH_AST_H
#define BERRYMATH_AST_H

#include <string>
#include <vector>
#include "lex.h"
using std::string;
using std::vector;

namespace BM {
    class AST {
    public:
        AST() : root(nullptr), script(""), child(false), baseLine(0), lexer(script), byCache(false) { }
        AST(const string& s) : root(nullptr), script(s), child(false), baseLine(0), lexer(script), byCache(false) { }
        void open(const string& s) { script = s;lexer.open(script);byCache = false; }
        void parse();
        void clear() {
            if (root) delete root;
            root = nullptr;
        }
        string exportByString();
        bool Export(string filename = "script.bmast");
        void import(string filename = "script.bmast");
        void importByString(string);
        ~AST() { if (!child) delete root; }
    private:
        static void trim(string& s) {
            s.erase(0, 1);
            s.erase(s.length() - 1, 1);
        }
        AST(const string& s, UL l) : root(nullptr), script(s), baseLine(l), child(true), lexer(script), byCache(false) { }
        class node {
        public:
            node() : v(""), l(0) { }
            node(string t) : v(t), l(0) { }
            node(string t, UL i) : v(t), l(i) { }
            inline string value() { return v; }
            inline void value(string c) { v = c; }
            inline UL line() { return l; }
            inline void line(UL t) { l = t; }
            inline node* get(long index) {
                if (index < 0) index += children.size();
                return children[index % children.size()];
            }
            inline UL length() { return children.size(); }
            node& operator[](long index) { return *get(index); }
            void insert(node* n) { children.push_back(n); }
            void insert(string v, UL l) { children.push_back(new node(v, l)); }
            string exportByString();
            ~node() {
                for (auto iter = children.begin(); iter != children.end(); iter++) {
                    if (*iter) delete (*iter);
                }
            }
        private:
            string v;
            UL l;
            vector<node*> children;
        };
        bool child;
        bool byCache;
        UL baseLine;
        node* root;
        string script;
        Lexer lexer;
        Lexer astLexer;
        static inline UL priority(const string&);
#define CHECK(astName) \
    if (astName->root->value() == "bad-tree") { \
        if (root) delete root; \
        root = astName->root; \
        delete astName; \
        return; \
    }
    };;
}


#endif //BERRYMATH_AST_H
