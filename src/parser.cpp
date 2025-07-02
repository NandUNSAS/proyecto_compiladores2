#include "parser.h"
#include <iostream>

using namespace std;

bool Parser::isAtEnd() {
    return pos >= tokens.size();
}

Token Parser::peek(int offset) {
    if (pos + offset >= tokens.size()) {
        return Token(TokenType::UNKNOWN, "");
    }
    return tokens[pos + offset];
}

bool Parser::match(TokenType type, const string& value) {
    if (isAtEnd()) return false;
    if (tokens[pos].type == type && (value.empty() || tokens[pos].value == value)) {
        pos++;
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const string& value) {
    if (match(type, value)) return tokens[pos - 1];

    cerr << "Esperando símbolo '" << value << "', token actual: tipo=" 
         << static_cast<int>(peek().type) << ", valor=\"" << peek().value << "\"" << endl;

    throw runtime_error("Token inesperado. Se esperaba tipo=" + to_string(static_cast<int>(type)) + 
                        " con valor=\"" + value + "\", pero se encontró tipo=" + 
                        to_string(static_cast<int>(peek().type)) + " con valor=\"" + peek().value + "\".");
}

ASTNodePtr Parser::parseStructDeclaration() {
    auto node = make_shared<StructDeclaration>();
    expect(TokenType::KEYWORD, "struct");
    node->name = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::SYMBOL, "{");

    while (!match(TokenType::SYMBOL, "}")) {
        StructField field;

        // Puede ser int, char, struct, etc.
        if (peek().value == "struct") {
            advance();  // consumir 'struct'
            field.type = "struct " + expect(TokenType::IDENTIFIER).value;
        } else {
            field.type = expect(TokenType::KEYWORD).value;
        }

        // Contar punteros: *, **, etc.
        while (match(TokenType::OPERATOR, "*")) {
            field.pointerLevel++;
        }

        // Nombre del campo
        field.name = expect(TokenType::IDENTIFIER).value;

        expect(TokenType::SYMBOL, ";");

        node->fields.push_back(field);
    }

    expect(TokenType::SYMBOL, ";");
    return node;
}


ASTNodePtr Parser::parseVariableDeclaration() {
    string type = expect(TokenType::KEYWORD).value; // int o struct
    string structName;

    if (type == "struct") {
        structName = expect(TokenType::IDENTIFIER).value;
    }

    int pointerLevel = 0;
    while (match(TokenType::OPERATOR, "*")) {
        pointerLevel++;
    }

    string name = expect(TokenType::IDENTIFIER).value;

    if (match(TokenType::OPERATOR, "=")) {
        if (match(TokenType::SYMBOL, "(")) {
            expect(TokenType::KEYWORD, "struct");
            expect(TokenType::IDENTIFIER);
            expect(TokenType::OPERATOR, "*");
            expect(TokenType::SYMBOL, ")");
            expect(TokenType::KEYWORD, "malloc");
            expect(TokenType::SYMBOL, "(");
            expect(TokenType::KEYWORD, "sizeof");
            expect(TokenType::SYMBOL, "(");
            expect(TokenType::KEYWORD, "struct");
            expect(TokenType::IDENTIFIER);
            expect(TokenType::SYMBOL, ")");
            expect(TokenType::SYMBOL, ")");
            expect(TokenType::SYMBOL, ";");

            auto mallocNode = make_shared<MallocAssignment>();
            mallocNode->varName = name;
            mallocNode->mallocType = structName;
            return mallocNode;
        } else {
            string value = expect(TokenType::LITERAL).value;
            expect(TokenType::SYMBOL, ";");

            auto var = make_shared<VariableDeclaration>();
            var->type = (structName.empty() ? type : "struct " + structName);
            var->pointerLevel = pointerLevel;
            var->name = name;
            return var;
        }
    }

    expect(TokenType::SYMBOL, ";");

    auto var = make_shared<VariableDeclaration>();
    var->type = (structName.empty() ? type : "struct " + structName);
    var->pointerLevel = pointerLevel;
    var->name = name;
    return var;
}



ASTNodePtr Parser::parseFunctionDefinition() {
    string returnType = expect(TokenType::KEYWORD).value;
    string name = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::SYMBOL, "(");
    expect(TokenType::SYMBOL, ")");
    expect(TokenType::SYMBOL, "{");

    auto func = make_shared<FunctionDefinition>();
    func->returnType = returnType;
    func->name = name;

    while (!match(TokenType::SYMBOL, "}")) {
        func->body.push_back(parseStatement());
    }

    return func;
}

ASTNodePtr Parser::parseReturnStatement() {
    expect(TokenType::KEYWORD, "return");
    string value = expect(TokenType::LITERAL).value;
    expect(TokenType::SYMBOL, ";");

    auto node = make_shared<ReturnStatement>();
    node->value = value;
    return node;
}

ASTNodePtr Parser::parseFreeCall() {
    expect(TokenType::KEYWORD, "free");
    expect(TokenType::SYMBOL, "(");
    string name = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::SYMBOL, ")");

    expect(TokenType::SYMBOL, ";");

    auto node = make_shared<FreeCall>();
    node->varName = name;
    return node;
}

ASTNodePtr Parser::parseFieldAssignment() {
    string structVar = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::OPERATOR, "->");
    string field = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::OPERATOR, "=");
    string value = expect(TokenType::LITERAL).value;
    expect(TokenType::SYMBOL, ";");

    auto node = make_shared<FieldAssignment>();
    node->structVar = structVar;
    node->field = field;
    node->value = value;
    return node;
}

ASTNodePtr Parser::parseStatement() {
    if (peek().value == "struct" && peek(1).type == TokenType::IDENTIFIER && peek(2).value == "{") {
        return parseStructDeclaration();
    }

    // Detectar posibles declaraciones (int, char, struct, etc.)
    if (peek().type == TokenType::KEYWORD &&
        (peek().value == "int" || peek().value == "char" || peek().value == "void" || peek().value == "struct")) {
        
        int offset = 1;

        // Si es struct, saltar también el nombre del struct
        if (peek().value == "struct") {
            if (peek(1).type != TokenType::IDENTIFIER)
                throw runtime_error("Se esperaba el nombre del struct después de 'struct'");
            offset++;
        }

        // Contar asteriscos (punteros)
        while (peek(offset).type == TokenType::OPERATOR && peek(offset).value == "*") {
            offset++;
        }

        // Confirmar que el siguiente es un identificador (nombre de variable o función)
        if (peek(offset).type == TokenType::IDENTIFIER) {
            if (peek(offset + 1).value == "(") {
                return parseFunctionDefinition();
            } else {
                return parseVariableDeclaration();
            }
        }
    }

    if (peek().value == "return") {
        return parseReturnStatement();
    }

    if (peek().type == TokenType::IDENTIFIER && peek(1).value == "->") {
        return parseFieldAssignment();
    }

    if (peek().value == "free") {
        return parseFreeCall();
    }

    cerr << "Token no reconocido en parseStatement(): tipo=" << static_cast<int>(peek().type)
         << ", valor=" << peek().value << endl;
    throw runtime_error("Token inesperado en línea de parseo.");
}


vector<ASTNodePtr> Parser::parse() {
    return parseProgram();
}

vector<ASTNodePtr> Parser::parseProgram() {
    vector<ASTNodePtr> program;
    while (!isAtEnd()) {
        //Llama a la función parseStatement(), que analiza y construye el nodo AST correspondiente a una instrucción o declaración completa. 
        program.push_back(parseStatement());
    }
    return program;
}

void Parser::advance() {
    if (!isAtEnd()) pos++;
}


