// ===== ast.h =====
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual string toString(int indent = 0) const = 0;
};
using ASTNodePtr = shared_ptr<ASTNode>;

struct BinaryExpression : public ASTNode {
    ASTNodePtr left, right;
    string op;

    string toString(int indent = 0) const override {
        string pad(indent, ' ');
        return pad + "BinaryExpression: " + op + "\n" +
               left->toString(indent + 2) +
               right->toString(indent + 2);
    }
};

struct DereferenceExpression : public ASTNode {
    ASTNodePtr expression;

    string toString(int indent = 0) const override {
        return string(indent, ' ') + "Dereference:\n" + expression->toString(indent + 2);
    }
};

struct VariableReference : public ASTNode {
    string name;

    string toString(int indent = 0) const override {
        return string(indent, ' ') + "Variable: " + name + "\n";
    }
};

struct Literal : public ASTNode {
    string value;

    string toString(int indent = 0) const override {
        return string(indent, ' ') + "Literal: " + value + "\n";
    }
};

struct StructField {
    string type;
    string name;
    int pointerLevel = 0;
};

struct StructDeclaration : public ASTNode {
    string name;
    vector<StructField> fields;

    string toString(int indent = 0) const override;
};



struct MallocAssignment : public ASTNode {
    string varName;
    string mallocType;

    string toString(int indent = 0) const override;
};


struct FreeCall : public ASTNode {
    string varName;

    string toString(int indent = 0) const override;
};

struct ReturnStatement : public ASTNode {
    string value;

    string toString(int indent = 0) const override;
};

struct FunctionDefinition : public ASTNode {
    string returnType;
    string name;
    vector<ASTNodePtr> body;

    string toString(int indent = 0) const override;
};
// ast.h

struct VariableDeclaration : public ASTNode {
    string type;
    bool isPointer = false;
    string name;
    string value;
    int pointerLevel = 0;


    string toString(int indent = 0) const override;  // ✅ Solo declaración
};


struct FieldAssignment : public ASTNode {
    string structVar;  // 🔴 NECESARIO
    string field;      // 🔴 NECESARIO
    string value;
    string toString(int indent = 0) const override; 
};

#endif
