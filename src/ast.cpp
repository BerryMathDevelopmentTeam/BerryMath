#include "ast.h"

void BM::ast::parse() {
    Lexer lexer(script);
    auto token = lexer.get();
    switch (token.t) {
        case Lexer::LET_TOKEN:
            break;
    }
}