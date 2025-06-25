#include "ast.h"

// Las funciones toString ya están implementadas en ast.h con la palabra clave `override`.
// Si prefieres separar las definiciones en `ast.cpp`, aquí te muestro cómo sería:

string StructDeclaration::toString(int indent) const {
    string pad(indent, ' ');
    string out = pad + "StructDeclaration: " + name + "\n";
    for (const auto& field : fields) {
        out += pad + "  Field: " + field.type + string(field.pointerLevel, '*') + " " + field.name + "\n";
    }
    return out;
}

string VariableDeclaration::toString(int indent) const {
    string pad(indent, ' ');
    string ptr = isPointer ? "*" : "";
    return pad + "VariableDeclaration: " + type + string(pointerLevel, '*') + " " + name;

}

string MallocAssignment::toString(int indent) const {
    string pad(indent, ' ');
    return pad + "MallocAssignment: " + varName + " = malloc(" + mallocType + ")";
}


string FieldAssignment::toString(int indent) const {
    string pad(indent, ' ');
    return pad + "FieldAssignment: " + structVar + "->" + field + " = " + value;
}

string FreeCall::toString(int indent) const {
    string pad(indent, ' ');
    return pad + "FreeCall: free(" + varName + ")";
}

string ReturnStatement::toString(int indent) const {
    string pad(indent, ' ');
    return pad + "ReturnStatement: " + value;
}

string FunctionDefinition::toString(int indent) const {
    string pad(indent, ' ');
    string out = pad + "FunctionDefinition: " + returnType + " " + name + "\n";
    for (const auto& stmt : body) {
        out += stmt->toString(indent + 2) + "\n";
    }
    return out;
}
