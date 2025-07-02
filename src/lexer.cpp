// lexer.cpp
#include "lexer.h"
#include <cctype>
#include <algorithm>
#include <sstream>

bool Lexer::isOperatorChar(char c) {
    return c == '*' || c == '&' || c == '=' || c == '+' || c == '-' || c == '.';
}

bool Lexer::isSymbol(char c) {
    return c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == ',';  //añadir []  para arreglos;
}

bool Lexer::isKeyword(const string& word) {
    return find(keywords.begin(), keywords.end(), word) != keywords.end();
}

vector<Token> Lexer::tokenize(const string& code) {
    vector<Token> tokens;
    int i = 0;

    while (i < code.length()) {
        //ignorara espacios en blanco.(tabulaciones, salto de linea)
        if (isspace(code[i])) {
            i++;
            continue;
        }
        //identifcadores y palabras clave, si es un KEYWORD (palabra clave o un IDENTIFER identificador).
        if (isalpha(code[i]) || code[i] == '_') {
            string word;
            while (isalnum(code[i]) || code[i] == '_') {
                word += code[i++];
            }
            if (isKeyword(word)) {
                tokens.push_back(Token(TokenType::KEYWORD, word));
            } else {
                tokens.push_back(Token(TokenType::IDENTIFIER, word));
            }
        }
        //verifica si es numero, por el momento sin comas.
        else if (isdigit(code[i])) {
            string number;
            while (isdigit(code[i])) {
                number += code[i++];
            }
            tokens.push_back(Token(TokenType::LITERAL, number));
        }
        //verifica si es una cadena
        else if (code[i] == '"' || code[i] == '\'') {
            char quote = code[i++];
            string str;
            while (i < code.length() && code[i] != quote) {
                str += code[i++];
            }
            i++; // cerrar comilla
            tokens.push_back(Token(TokenType::LITERAL, str));
        }

        //verificar operadores
        else if (isOperatorChar(code[i])) {
            string op;
            op += code[i++];
            // Para casos como '->'
            if (op == "-" && code[i] == '>') {
                op += code[i++];
            }
            tokens.push_back(Token(TokenType::OPERATOR, op));
        }

        // si es un simbolo (;,(,),{,})
        else if (isSymbol(code[i])) {
            string sym;
            sym += code[i++];
            tokens.push_back(Token(TokenType::SYMBOL, sym));
        }
        else {
            string unknown;
            unknown += code[i++];
            tokens.push_back(Token(TokenType::UNKNOWN, unknown));
        }
    }

    return tokens;
}
