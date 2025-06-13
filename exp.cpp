#include <iostream>
#include "exp.h"
using namespace std;



// Exp base class
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP: return "+";
        case MINUS_OP: return "-";
        case MUL_OP: return "*";
        case DIV_OP: return "/";
        case LT_OP: return "<";
        case LE_OP: return "<=";
        case EQ_OP: return "==";
        case GE_OP: return ">=";
        case GT_OP: return ">";
        case NE_OP: return "!=";
        case AND_OP: return "&&";
        case OR_OP: return "||";
        default: return "?";
    }
}

string Exp::unopToChar(UnaryOp op) {
    switch (op) {
        case INC_PRE_OP: return "++";
        case DEC_PRE_OP: return "--";
        case INC_POST_OP: return "++";
        case DEC_POST_OP: return "--";
        case NOT: return "!";
        default: return "?";
    }
}

// BinaryExp
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op) : left(l), right(r), op(op) {}
BinaryExp::~BinaryExp() {}


// UnaryExp
UnaryExp::UnaryExp(Exp* l, UnaryOp op) : left(l), op(op) {}
UnaryExp::~UnaryExp() {}


// NumberExp
NumberExp::NumberExp(int v) : value(v) {}
NumberExp::~NumberExp() {}


// BoolExp
BoolExp::BoolExp(bool v) : value(v) {}
BoolExp::~BoolExp() {}


// IdentifierExp
IdentifierExp::IdentifierExp(const std::string& n) : name(n) {}
IdentifierExp::~IdentifierExp() {}


// FCallExp


// Stm base class
Stm::~Stm() {}

// AssignStatement
AssignStatement::AssignStatement(std::string id, Exp* e) : id(id), rhs(e) {}
AssignStatement::~AssignStatement() {}

// PrintStatement
PrintfStatement::PrintfStatement(string s) : stringsito(s) {}
PrintfStatement::PrintfStatement(){}
PrintfStatement::~PrintfStatement() {}

// IfStatement
IfStatement::IfStatement(Exp* condition, Block* then, Block* els)
    : condition(condition), then(then), els(els) {}
IfStatement::~IfStatement() {}

// WhileStatement
WhileStatement::WhileStatement(Exp* condition, Block* b)
    : condition(condition), b(b) {}
WhileStatement::~WhileStatement() {}

// VarDec
VarDec::VarDec(string type, list<string> vars) : type(type), vars(vars) {}
VarDec::~VarDec() {}

// VarDecList
VarDecList::VarDecList() {}
VarDecList::~VarDecList() {
    for (auto v : vardecs) delete v;
}
void VarDecList::add(VarDec* vardec) { vardecs.push_back(vardec); }

// ArgList
ArgList::ArgList() {}
ArgList::~ArgList() {
    for (auto v : aexps) delete v;
}
void ArgList::add(Exp* aexp) { aexps.push_back(aexp); }

// StatementList
StatementList::StatementList() {}
StatementList::~StatementList() {
    for (auto s : stms) delete s;
}
void StatementList::add(Stm* stm) { stms.push_back(stm); }

// Block
Block::Block(VarDecList* varDecs, StatementList* stmts)
    : varDecs(varDecs), stmts(stmts) {}
Block::~Block() {
    delete varDecs;
    delete stmts;
}

// ParamDec
ParamDec::ParamDec(string type, string id) : type(type), id(id) {}
ParamDec::~ParamDec() {}

// ParamDecList
ParamDecList::ParamDecList() {}
ParamDecList::~ParamDecList() {
    for (auto p : params) delete p;
}
void ParamDecList::add(ParamDec* param) { params.push_back(param); }

// FunDec
FunDec::FunDec(std::string type, std::string id, ParamDecList* params, Block* block)
    : type(type), id(id), params(params), block(block) {}
FunDec::~FunDec() {
    delete params;
    delete block;
}

// FunDecList
FunDecList::FunDecList() {}
FunDecList::~FunDecList() {
    for (auto f : funDecs) delete f;
}
void FunDecList::add(FunDec* funDec) { funDecs.push_back(funDec); }

// Program
Program::Program(FunDecList* funcList, FunDec* mainFunction)
    : funcList(funcList), mainFunction(mainFunction) {}
Program::~Program() {
    delete funcList;
    delete mainFunction;
}