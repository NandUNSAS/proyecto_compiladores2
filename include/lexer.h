// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include "token.h"

using namespace std;

class Lexer {
private:
    vector<string> keywords = {
        "struct", "malloc", "free", "int", "char", "void", "return", "NULL", "sizeof"
    };

    bool isOperatorChar(char c);
    bool isSymbol(char c);
    bool isKeyword(const string& word);

public:
    vector<Token> tokenize(const string& code);
};

#endif
