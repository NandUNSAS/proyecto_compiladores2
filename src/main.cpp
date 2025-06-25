#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

using namespace std;

void printAST(const vector<ASTNodePtr>& ast, int indent = 0) {
    for (const auto& node : ast) {
        cout << node->toString(indent) << endl;
    }
}

int main() {
    ifstream file("../entrada.txt");
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo codigo.c" << endl;
        return 1;
    }

    // Leer archivo completo
    stringstream buffer;
    buffer << file.rdbuf();
    string codigo = buffer.str();

    // Análisis léxico
    Lexer lexer;
    vector<Token> tokens = lexer.tokenize(codigo);
    
    for (const auto& t : tokens) {
    cout << "Token: " << static_cast<int>(t.type) << " -> " << t.value << endl; 
    }

    // Análisis sintáctico
    try {
        Parser parser(tokens);
        vector<ASTNodePtr> ast = parser.parseProgram();


        // Imprimir AST
        cout << "== AST GENERADO ==" << endl;
        printAST(ast);

    } catch (const exception& e) {
        cerr << "Error durante análisis sintáctico: " << e.what() << endl;
    }

    return 0;
}

