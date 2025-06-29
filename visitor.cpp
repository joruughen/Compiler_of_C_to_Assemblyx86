#include <iostream>
#include "exp.h"
#include "visitor.h"

#include "type_visitor.hh"
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}


int FunDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// int Program::accept(Visitor* v) {
//     v->visit(this);
//     return 0;
// }

int FunDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int Block::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int ForStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ReturnStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int FCallExp::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

ImpType BinaryExp::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

ImpType NumberExp::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

ImpType BoolExp::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

ImpType IdentifierExp::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

ImpType FCallExp::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void VarDec::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void FunDec::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void Program::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void FunDecList::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void AssignStatement::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void PrintStatement::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void IfStatement::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void WhileStatement::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void VarDecList::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void StatementList::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void Block::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void ForStatement::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}

void ReturnStatement::accept(TypeVisitor *visitor) {
    return visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) cout << "true";
    else cout << "false";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

int PrintVisitor::visit(FCallExp *exp) {
    cout<<exp->id<<"(";
    for(auto i: exp->params){
        i->accept(this);
        if(i != exp->params.back()) cout << ", ";
    }
    cout<<")";
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    cout << "printf(\"";
    cout<<stm->s;
    cout<<"\"";
    for (auto e: stm->param_list) {
        cout<<", ";
        e->accept(this);
    }
    cout << ");";
}

void PrintVisitor::visit(IfStatement* stm) {
    cout << "if(";
    stm->condition->accept(this);
    cout << "){" << endl;
    stm->then->accept(this);
    cout << "}";
    if(stm->els){
    cout << "else{" << endl;
    stm->els->accept(this);
    }
    cout << "}";
}

void PrintVisitor::visit(ReturnStatement *stm) {
    cout<<"return ";
    stm->e->accept(this);
    cout<<";";
}


void PrintVisitor::imprimir(Program *program) {
    program->fundecs->accept(this);
    cout<<"\n";
    program->main->accept(this);
}

void PrintVisitor::visit(FunDecList *fl) {
    for (auto e: fl->fundecs) {
        e->accept(this);
    }
}

void PrintVisitor::visit(FunDec *fl) {
    cout<<fl->type<< " ";
    cout<<fl->id;
    cout<<"(";
    string param="";
    for (int i = 0; i < fl->param_ids.size(); i++) {
        param += fl->param_types[i] + " "+ fl->param_ids[i];
        param += ", ";
    }
    if (param != "") {
        param.pop_back(); //" "
        param.pop_back();// ","
    }
    cout<<param;
    cout<<")";
    cout<<"{\n";
    fl->block->accept(this);
    cout<<"}\n";
}

void PrintVisitor::visit(WhileStatement* stm){
    cout << "while(";
    stm->condition->accept(this);
    cout << "){" << endl;
    stm->b->accept(this);
    cout << "}";
}

void PrintVisitor::visit(ForStatement *stm) {
    cout << "for(";
    if (stm->dcl != nullptr) {
        cout<< stm->dcl->type << " ";
    }
    stm->init->accept(this);
    stm->condition->accept(this);
    cout<<";";
    stm->update->accept(this);
    cout<<"){";
    stm->body->accept(this);
    cout<<"}\n";
}

void PrintVisitor::visit(VarDec* stm){
    cout << stm->type;
    cout << " ";
    for(auto i: stm->vars){
        cout << i;
        if(i != stm->vars.back()) cout << ", ";
    }
    cout << ";";
}

void PrintVisitor::visit(VarDecList* stm){
    for(auto i: stm->vardecs){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(StatementList* stm){
    for(auto i: stm->stms){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Block* stm){
    stm->vardecs->accept(this);
    cout << endl;
    stm->slist->accept(this);
}

