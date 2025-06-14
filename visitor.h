#ifndef VISITOR_H
#define VISITOR_H
#include "exp.h"
#include <list>

class BinaryExp;
class NumberExp;
class BoolExp;
class IdentifierExp;
class AssignStatement;
class PrintfStatement;
class IfStatement;
class WhileStatement;
class VarDec;
class VarDecList;
class StatementList;
class Body;
class Program;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(IdentifierExp* exp) = 0;
    virtual void visit(AssignStatement* stm) = 0;
    virtual void visit(PrintfStatement* stm) = 0;
    virtual void visit(IfStatement* stm) = 0;
    virtual void visit(WhileStatement* stm) = 0;
    virtual void visit(VarDec* stm) = 0;
    virtual void visit(VarDecList* stm) = 0;
    virtual void visit(StatementList* stm) = 0;
    virtual void visit(Body* b) = 0;
};

class PrintVisitor : public Visitor {
public:
    void imprimir(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(IdentifierExp* exp) override;
    void visit(AssignStatement* stm) override;
    void visit(PrintfStatement* stm) override;
    void visit(IfStatement* stm) override;
    void visit(WhileStatement* stm) override;
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
};

#endif