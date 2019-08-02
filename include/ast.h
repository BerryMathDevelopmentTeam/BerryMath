#ifndef BERRYMATH_AST_H
#define BERRYMATH_AST_H

#include <string>
#include <regex>
#include <vector>
#include "lex.h"
#include "types.h"
using std::string;
using std::vector;

namespace BM {
    class AST {
    public:
        AST() : root(nullptr), script(""), child(false), baseLine(0), lexer(script), byCache(false) {
            script += "\n";
            std::regex pattern("//.*[$\n]", std::regex::icase);
            script = std::regex_replace(script, pattern, "\n");
            script += "\n;pass";
        }
        AST(const string& s) : root(nullptr), script(s), child(false), baseLine(0), lexer(script), byCache(false) {
            script += "\n";
            std::regex pattern("//.*[$\n]", std::regex::icase);
            script = std::regex_replace(script, pattern, "\n");
            script += "\n;pass";
        }
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
        string value() { return root->value(); }
        ~AST() { if (!child) delete root; }
    private:
        static void trim(string& s) {
            s.erase(0, 1);
            s.erase(s.length() - 1, 1);
        }
        static void replace(string& s, const string& a) {
            size_t pos;
            auto alen = a.length();
            while ((pos = s.find(a)) != string::npos) {
                s = s.replace(pos, alen, "");
            }
        }
        AST(const string& s, UL l) : root(nullptr), script(s), baseLine(l), child(true), lexer(script), byCache(false) {
            script += "\n";
            std::regex pattern("//.*[$\n]", std::regex::icase);
            script = std::regex_replace(script, pattern, "\n");
            script += "\n;pass";
        }
    public:
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
            void insert(node* n) { if (n) children.push_back(n); }
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
    if (astName->root && astName->root->value() == "bad-tree") { \
        if (root) delete root; \
        root = astName->root; \
        delete astName; \
        return; \
    }
    public:
        node* rValue() { return root; }
        UL line() {
            if (root) return root->line() + 1;
            return 0;
        }
    };

#define CHANGELINES(ast) lexer.l = ast->lexer.l + ast->baseLine;
}


#endif //BERRYMATH_AST_H
