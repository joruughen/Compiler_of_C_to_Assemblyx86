#ifndef EXP_H
#define EXP_H
#include "imp_value.hh"
#include "imp_type.hh"
#include <string>
#include <unordered_map>
#include <list>
#include "visitor.h"
class TypeVisitor;
using namespace std;
enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,LT_OP, LE_OP, EQ_OP, GT_OP,GE_OP, NE_OP, AND_OP, OR_OP };
enum UnaryOp { NOT_OP};
class Block;
class ImpValueVisitor;
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ImpType accept(TypeVisitor* visitor) = 0;
    virtual void accept(ImpValueVisitor* v) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};



class BinaryExp : public Exp {
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~BinaryExp() {
        delete left;
        delete right;
    }
    ImpType accept(TypeVisitor *visitor);
};

class NumberExp : public Exp {
public:
    int value;
    UnaryOp op;
    NumberExp(int v);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~NumberExp();
    ImpType accept(TypeVisitor *visitor);
};

class BoolExp : public Exp {
public:
    int value;
    UnaryOp op;
    BoolExp(bool v);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~BoolExp();
    ImpType accept(TypeVisitor *visitor);
};

class IdentifierExp : public Exp {
public:
    std::string name;
    UnaryOp op;
    IdentifierExp(const std::string& n);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~IdentifierExp();
    ImpType accept(TypeVisitor *visitor);
};


class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
    virtual void accept(ImpValueVisitor* v) = 0;
    virtual void accept(TypeVisitor* visitor) = 0;
};

class VarDec {
public:
    string type;
    list<string> vars;
    VarDec(string type, list<string> vars);
    int accept(Visitor* visitor);

    void accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
    ~VarDec();
};


class Block{
public:
    VarDecList* vardecs;
    StatementList* slist;
    Block(VarDecList* vardecs, StatementList* stms);
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    void accept(ImpValueVisitor* v);
    ~Block();
};


class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
    int accept(Visitor* visitor);
    void accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
    ~AssignStatement();
};

class PrintStatement : public Stm {
public:
    string s;
    vector<Exp*> param_list;
    PrintStatement(string s);
    PrintStatement(string s,vector<Exp*> param_list);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor *visitor);
    ~PrintStatement() {
        for (auto e: param_list) {
            delete e;
        }

    };
};


class IfStatement : public Stm {
public:
    Exp* condition;
    Block* then;
    Block* els;
    IfStatement(Exp* condition, Block* then, Block* els);
    int accept(Visitor* visitor);
    void accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
    ~IfStatement();
};
class WhileStatement : public Stm {
public:
    Exp* condition;
    Block* b;
    WhileStatement(Exp* condition, Block* b);
    int accept(Visitor* visitor);
    void accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
    ~WhileStatement() {
        delete b;
        delete condition;
    }
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList();
    void add(VarDec* vardec);
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    void accept(ImpValueVisitor* v);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    void accept(ImpValueVisitor* v);
    ~StatementList();
};

class ForStatement: public Stm {
public:
    VarDec* dcl;
    Stm* init;
    Exp* condition;
    Stm* update;
    Block* body;
    ForStatement(Stm* init, Exp* condition, Stm* update, Block* body);
    ForStatement(VarDec* dcl,Stm* init, Exp* condition, Stm* update, Block* body);
    ~ForStatement() {
        delete dcl;
        delete body;
        delete init;
        delete condition;
        delete update;
    }
    int accept(Visitor* visitor);
    void accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
};

class ReturnStatement: public Stm {
public:
    Exp* e;
    ReturnStatement(Exp* e):e(e){};
    ~ReturnStatement(){delete e;};
    int accept(Visitor* visitor);
    void accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
};

class FunDec {
public:
    string id;
    string type;
    vector<string> param_ids;
    vector<string> param_types;
    Block* block;
    FunDec() = default;
    ~FunDec() {
        delete block;
    };
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor);
    void accept(ImpValueVisitor* v);
};

class FCallExp : public Exp {
public:
    string id;
    list<Exp*> params;
    FCallExp(list<Exp*> argumentos, string nombre):id(nombre),params(argumentos){};
    ~FCallExp() {
        for (auto e: params) {
            delete e;
        }
    };
    int accept(Visitor* visitor);
    ImpType accept(TypeVisitor *visitor);
    void accept(ImpValueVisitor* v);
};

class FunDecList{
public:
    list<FunDec*> fundecs;
    void add(FunDec* fundec) {
        fundecs.push_back(fundec);
    };
    int accept(Visitor* visitor);
    FunDecList() = default;
    ~FunDecList() {
        for (auto e: fundecs) {
            delete e;
        }
    };
    void accept(TypeVisitor* visitor);

};

class Program {
public:
    list<string> includes;
    FunDecList* fundecs;
    FunDec* main;
    Program() = default;
    ~Program() = default;
    // int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
};



#endif // EXP_H