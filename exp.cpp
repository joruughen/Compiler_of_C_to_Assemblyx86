#include <iostream>
#include "exp.h"
using namespace std;
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}
BoolExp::BoolExp(bool v):value(v) {}
IdentifierExp::IdentifierExp(const string& n):name(n) {}
PrintStatement::PrintStatement(string s):s(s){}
PrintStatement::PrintStatement(string s, vector<Exp*> param_list):s(s),param_list(param_list){}
ForStatement::ForStatement(Stm* init, Exp* cond, Stm* next, Block* b):init(init),condition(cond),update(next),body(b){}
ForStatement::ForStatement(VarDec* dcl,Stm* init, Exp* cond, Stm* next, Block* b):dcl(dcl),init(init),condition(cond),update(next),body(b){}
Exp::~Exp() {}
NumberExp::~NumberExp() { }
BoolExp::~BoolExp() { }
IdentifierExp::~IdentifierExp() { }

AssignStatement::AssignStatement(string id, Exp* e): id(id), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}

IfStatement::IfStatement(Exp* c, Block* t, Block* e): condition(c), then(t), els(e) {}
IfStatement::~IfStatement() {
    delete condition;
    delete then;
    delete els;
}
WhileStatement::WhileStatement(Exp* c, Block* t): condition(c), b(t) {}



VarDec::VarDec(string type, list<string> vars): type(type), vars(vars) {}
VarDec::~VarDec() {}

VarDecList::VarDecList(): vardecs() {}
void VarDecList::add(VarDec* v) {
    vardecs.push_back(v);
}
VarDecList::~VarDecList() {
    for (auto v: vardecs) {
        delete v;
    }
}

StatementList::StatementList(): stms() {}
void StatementList::add(Stm* s) {
    stms.push_back(s);
}

StatementList::~StatementList() {
    for (auto s: stms) {
        delete s;
    }
}

Block::Block(VarDecList* v, StatementList* s): vardecs(v), slist(s) {}
Block::~Block() {
    delete vardecs;
    delete slist;
}





Stm::~Stm() {}
string Exp::binopToChar(BinaryOp op) {
    string  c;
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case EQ_OP: c = "=="; break;
        case GT_OP: c = ">"; break;
        case GE_OP: c = ">="; break;
        case NE_OP: c = "!="; break;
        case AND_OP: c = "&&"; break;
        case OR_OP: c = "||"; break;
        default: c = "$";
    }
    return c;
}