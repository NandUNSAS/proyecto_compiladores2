#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>
#include <stdexcept>

using namespace std;

class Parser {
private:
    vector<Token> tokens;
    size_t pos = 0;

    bool match(TokenType type, const string& value = "");
    Token expect(TokenType type, const string& value = "");
    Token peek(int offset = 0);
    bool isAtEnd();
    void advance();   // ✔ declarar aquí
   

    // Gramática
    ASTNodePtr parseStructDeclaration();
    ASTNodePtr parseVariableDeclaration();
    ASTNodePtr parseMallocAssignment();
    ASTNodePtr parseFieldAssignment();
    ASTNodePtr parseFreeCall();
    ASTNodePtr parseStatement();
    ASTNodePtr parseFunctionDefinition();
    ASTNodePtr parseReturnStatement();
    ASTNodePtr parseExpression();
    ASTNodePtr parsePrimary();
    

public:
    Parser(const vector<Token>& tokens) : tokens(tokens) {}
    vector<ASTNodePtr> parse();
    vector<ASTNodePtr> parseProgram();
};

#endif
