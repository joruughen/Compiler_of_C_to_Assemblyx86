#ifndef EXP_H
#define EXP_H
#include "imp_value.h"
#include "imp_type.h"
#include <string>
#include <unordered_map>
#include <list>
#include "visitor.h"
using namespace std;
enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, LT_OP, LE_OP, EQ_OP, GE_OP, GT_OP, NE_OP, AND_OP, OR_OP };
enum UnaryOp { INC_PRE_OP, DEC_PRE_OP, INC_POST_OP, DEC_POST_OP, NOT};

// class Body;
class Block;
class ImpValueVisitor;
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ImpValue accept(ImpValueVisitor* v) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
    static string unopToChar(UnaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~BinaryExp();
};

class UnaryExp : public Exp {
public:
    Exp *left;
    UnaryOp op;
    UnaryExp(Exp* l, UnaryOp op);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~UnaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~BoolExp();
};

class IdentifierExp : public Exp {
public:
    std::string name;
    IdentifierExp(const std::string& n);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~IdentifierExp();
};

class ArgList{
public:
    list<Exp*> aexps;
    ArgList();
    void add(Exp* aexp);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~ArgList();
};


class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
    virtual void accept(ImpValueVisitor* v) = 0;
};


class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~AssignStatement();
};

class PrintfStatement : public Stm {
public:
    string stringsito;
    ArgList* args;
    PrintfStatement(string s);
    PrintfStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~PrintfStatement();
};


class IfStatement : public Stm {
public:
    Exp* condition;
    Block* then;
    Block* els;
    IfStatement(Exp* condition, Block* then, Block* els);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~IfStatement();
};
class WhileStatement : public Stm {
public:
    Exp* condition;
    Block* b;
    WhileStatement(Exp* condition, Block* b);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~WhileStatement();
};


class VarDec {
public:
    string type;
    list<string> vars;
    VarDec(string type, list<string> vars);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~VarDec();
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList();
    void add(VarDec* vardec);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~VarDecList();
};




class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~StatementList();
};


// class Body{
// public:
//     VarDecList* vardecs;
//     StatementList* slist;
//     Body(VarDecList* vardecs, StatementList* stms);
//     int accept(Visitor* visitor);
//     void accept(ImpValueVisitor* v);
//     ~Body();
// };



class Block {
public:
    VarDecList* varDecs;
    StatementList* stmts;

    Block(VarDecList* varDecs, StatementList* stmts);
    ~Block();
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};


class ParamDec {
public:
    string type;
    string id;

    ParamDec(string type, string id);
    ~ParamDec();
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};

class ParamDecList {
public:
    list<ParamDec*> params;

    ParamDecList();
    ~ParamDecList();
    void add(ParamDec* param);
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};


class FunDec {
public:
    string type;
    string id;
    ParamDecList* params;
    Block* block;
    FunDec(std::string type, std::string id, ParamDecList* params, Block* block);
    ~FunDec();
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};


class FunDecList {
public:
    list<FunDec*> funDecs;

    FunDecList();
    ~FunDecList();
    void add(FunDec* funDec);
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};

class FCallExp : public Exp {

public:

    string nombre;

    list<Exp*> argumentos;

    FCallExp(string nombre, list<Exp*> argumentos):nombre(nombre),argumentos(argumentos){};

    ~FCallExp(){};

    int accept(Visitor* visitor);

};



class Program {
public:
    string stdio_h;
    FunDecList* funcList;
    FunDec* mainFunction;
    Program(FunDecList* funcList, FunDec* mainFunction);
    ~Program();
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};



#endif // EXP_H