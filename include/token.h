// token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

enum class TokenType {
    KEYWORD, IDENTIFIER, OPERATOR, LITERAL, SYMBOL, UNKNOWN
};

struct Token {
    TokenType type;
    string value;

    Token(TokenType t, string v) : type(t), value(v) {}
};

#endif
